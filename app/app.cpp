#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <errno.h>
#include "app.hpp"
#include "../http_parser/http_request_parser.hpp"
#include "../http_parser/http_response.hpp"
#include <sstream>
#include <unordered_map>
#ifdef CJANGO_DYNLOAD
#include <FileWatcher.h>
#endif

#ifdef DEBUG
// See: https://github.com/gabime/spdlog/issues/154
#include <memory>
#include <spdlog/spdlog.h>

// std::unordered_map<std::string, std::shared_ptr<spdlog::logger>> loggers;

#endif

/**
* @mainpage Cjango-Unchained
* @section Introduction
*
* The Cjango-Unchained is a lightweight C++ web app framework. This documentation
* describes Cjango's internal programming interfaces,
* including ones which you will use when you write your original callback functions.
*/

constexpr unsigned int BUFFER_MAXSIZE = 4096;
const std::string logskt = "skt";
enum HandleRequestOp { NO_CHANGE, CLR_FDSET, CLOSE_SOCK };

void App::print_routes()
{
    std::cout << "Hello app!" << std::endl;
}

void error_exit(std::string msg, int sock=0)
{
    // _DEBUG(msg);
    perror(msg.c_str());
    if (sock)
        close(sock);
    exit(1);
}

void App::worker(int clntSock, std::string strRequest)
{
    try {
        _SPDLOG(logskt, info, "Worker thread invoked for socket {}", clntSock);
        http::HttpRequest request("");
        // _SPDLOG(logskt, info, "request: {}", strRequest); // long
        try {
            http::HttpRequestParser parser;
            std::stringstream ss;
            ss << strRequest;
            //std::cout << ss << std::endl;
            request = parser.parse(ss);

            //std::unordered_map<std::string, std::string> body = parser.parse_body(ss, );
            // _SPDLOG(logskt, info, "{}", request);  // FIXME _DEBUG for multi lines
            _SPDLOG(logskt, info, "finished request");
            for (auto entry : request.get_parameters()) {
                _SPDLOG(logskt, info, "{}:{}", entry.first, entry.second);
            }
        } catch (const char *e) {
        // if you throw a char* instead of exception class, that comes as const char*
            std::cout << e << std::endl;
        }
        http::HttpResponse response = router.get_http_response(request);
        string resp = response.to_string();
        // If static files served, resp is gibberish
        _SPDLOG(logskt, debug, "resp.length: ", resp.length());

        if (send(clntSock, resp.c_str(), resp.length(), 0) < 0) {
            _SPDLOG(logskt, error, "Failed to send response on socket: {}", clntSock);
        }

        /* It is assumed that at this point the server has done its job,
         * so it is closing the client socket */
        _SPDLOG(logskt, info, "Worker thread has done its job, closing socket {}", clntSock);
        close(clntSock);
    } catch (const std::exception& e) {
        _SPDLOG(logskt, info, "Exceptions caught in worker thread: {}. Closing socket anyway", e.what());
        if (clntSock)
            close(clntSock);
    }
}

int App::handle_request(int clntSock)
{
    HandleRequestOp ret = NO_CHANGE; /* return value */
    std::string contents;
    char buff[BUFFER_MAXSIZE];

    /* loop recv() from client socket for request contents */
    while (1) {
        memset(buff, 0, BUFFER_MAXSIZE);
        int rc = recv(clntSock, buff, sizeof(buff), 0);
        _SPDLOG(logskt, info, "From socket {}  recv returns value: {}", clntSock, rc);

        if (rc > 0) {
            /* recv() returns > 0, some data is read */

            /*
             mengdi: fix off by 1 error for content materials by changing
             std::begin(buff)+rc-1 to std::begin(buff)+rc
             */
            _SPDLOG(logskt, info, "rc >0 ");
            contents += std::string(std::begin(buff), std::begin(buff) + rc);
            continue; /* continue recv-ing till all data is read */
        }
        else if (rc == 0) {
            /* recv() returns 0, client has closed this connection */
            _SPDLOG(logskt, info, "Client has closed connection on socket {}", clntSock);
            ret = CLOSE_SOCK;
            break;
        }
        else if (rc < 0) {
            /* recv() returns < 0, error unless it's EWOULDBLOCK */
            if (errno != EWOULDBLOCK) {
                _SPDLOG(logskt, info, "ERROR: Failed to receive on socket {}", clntSock);
                ret = CLOSE_SOCK;
            }
            else {
                /* no data for now, try later... no need to close socket */
                _SPDLOG(logskt, info, "May try again later on socket {}", clntSock);
            }
            break;
        }
    } /* END: while(1) */

    /* At this point, if there is no need to close socket,
     * there might be data for us to process, start a new
     * thread to do the job if necessary */
    if (ret != CLOSE_SOCK && contents.size() > 0) {
        std::thread t(&App::worker, this, clntSock, contents);
        auto id = t.get_id();
        t.detach();
        std::ostringstream ss;
        ss << id;
        _SPDLOG(logskt, info, "Created and detached new thread {} for socket {}", ss.str(), clntSock);

        /* a new thread is started to handle the request,
         * it will eventually CLOSE the client socket, so
         * we must inform the server process to update the
         * fd_set so it won't need to select() on it */
        ret = CLR_FDSET;
    }

    return ret;
}

#ifdef CJANGO_DYNLOAD
#include <chrono>
void App::monitor_file_change() {
    for (;;) {
        this->fileWatcher.update(is_file_updated);
        if(is_file_updated) {
            router.load_url_pattern_from_file();
            is_file_updated = false;
        }
        // prevent busy looping -- should take a rest for a while
        // This would reduce CPU usage of ./testrun from 100% to 0.1%
        std::this_thread::sleep_for(1s);
    }
}

void App::spawn_monitor_thread() {
    // run a thread for checking file changes by every 1 second independently
    thread t(&App::monitor_file_change, this);
    auto monitor_id = t.get_id();
    t.detach();
    std::ostringstream ss;
    ss << monitor_id;
    _SPDLOG(logskt, info, "detached a new thread w/ monitor_id: {}", ss.str());
}

#endif

static void set_nonblocking(const int sock, const int servSock=0)
{
    int fl = fcntl(sock, F_GETFL);
    if (fl < 0)
        error_exit("Failed to get socket flags", servSock);

    if (fcntl(sock, F_SETFL, fl | O_NONBLOCK) < 0)
        error_exit("Failed to set socket flags", servSock);
}

void App::run(int port)
{
    _SPDLOG(logskt, info, "Invoked for port: {}", port);

    if (this->servSock >= 0) {
        _SPDLOG(logskt, info, "Already running on socket: {}", this->servSock);
        return;
    }

    /* open socket to listen to client connections */
    const int servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (servSock < 0)
        error_exit("Failed to create server socket");
    this->servSock = servSock;
    _SPDLOG(logskt, info, "Created server socket: {}", servSock);

    /* set socket to be reuseable */
    int reuse = 1;
    if (setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse)) < 0)
        error_exit("Failed to set socket reuseable", servSock);

    /* set socket to non-blocking */
    set_nonblocking(servSock, servSock);

    /* bind the socket */
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);
    if (::bind(servSock, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)
        error_exit("Failed to bind server address", servSock);

    /* listen */
    /** @bug if the number of opened sockets exceeds SOMAXCONN,
     Cjango gets "apr_socket_recv: Connection reset by peer (54)" error and halts immediately.
     On Mac OS X (default: 128), check by "sysctl -a | grep somax" and
     change it by "sudo sysctl -w kern.ipc.somaxconn=2048"
    */
    _SPDLOG(logskt, info, "Backlog of listen: {}", SOMAXCONN);
    if (listen(servSock, SOMAXCONN) < 0)
        error_exit("Failed to listen", servSock);

    /* initialize fd_set for select() */
    // If "struct fd_set allSocks", Ubuntu returned an typedef error
    fd_set allSocks;
    FD_ZERO(&allSocks);
    FD_SET(servSock, &allSocks);
    int maxSock = servSock;

    /* initialize timeout interval for select() */
    struct timeval timeout;
    timeout.tv_sec = 1; /* check select() every 1 second */
    timeout.tv_usec = 0;

#ifdef CJANGO_DYNLOAD
    spawn_monitor_thread();
#endif

    int heartBeat = 0; /* set to 0 if u don't want to print heartbeats */
    for (;;) {

        /* use a temporary fd_set for pulling from select() */
        fd_set tmpSocks;
        FD_ZERO(&tmpSocks);
        memcpy(&tmpSocks, &allSocks, sizeof(allSocks));

        /* call select() and wait till it returns for either socket events or timeout */
        int rc = select(maxSock + 1, &tmpSocks, NULL, NULL, &timeout);

        if (rc < 0)
            error_exit("Failed to select", servSock);

        /* nothing changes, timeout */
        if (rc == 0) {
            /* if heartBeat > 0, print sth to show it's alive */
            if (heartBeat == 60) {
                std::cout << "." << std::endl;
                fflush(stdout);
                heartBeat = 1;
            }
            else if (heartBeat > 0) {
                std::cout << ".";
                fflush(stdout);
                heartBeat++;
            }
            continue;
        } /* END: rc==0 */

        int nrSocks = rc; /* number of sockets changed */
        _SPDLOG(logskt, info, "Number of sockets readable: {}", nrSocks);
        for (int sd = 0; sd <= maxSock && nrSocks > 0; ++sd) {
            if (!FD_ISSET(sd, &tmpSocks))
                continue;

            /* found another readable socket */
            nrSocks--;

            if (sd == servSock) {
                _SPDLOG(logskt, info, "Server socket readable");
                int clntSock;
                do {
                    clntSock = accept(servSock, NULL, NULL);
                    if (clntSock < 0) {
                        if (errno != EWOULDBLOCK)
                            error_exit("Failed to accept", servSock);
                        break;
                    }
                    /* set clinet socket to non-blocking */
                    set_nonblocking(clntSock, servSock);

                    /* a new client connection accepted, add it to fd_set */
                    _SPDLOG(logskt, info, "New client connection: {}", clntSock);
                    FD_SET(clntSock, &allSocks);
                    if (clntSock > maxSock)
                        maxSock = clntSock;
                } while(clntSock >= 0);
            } /* END: if servSock readable */
            else {
                _SPDLOG(logskt, info, "Client socket {} readable", sd);
                int op = this->handle_request(sd);
                if (op != NO_CHANGE) {
                    if (op == CLOSE_SOCK) {
                        _SPDLOG(logskt, info, "Closing socket {}", sd);
                        close(sd);
                    }
                    FD_CLR(sd, &allSocks);
                    if (sd == maxSock) {
                        while(!FD_ISSET(maxSock, &allSocks))
                            maxSock--;
                    }
                }
            } /* END: else (client readable) */
        } /* END: loop sd */
    }  /* END: main loop */

    _SPDLOG(logskt, info, "Terminating server, closing socket {}", servSock);
    close(servSock);
}
