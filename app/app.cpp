#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include "app.hpp"

constexpr unsigned int BUFFER_MAXSIZE = 256;
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

static void worker1(int clntSock, Router router)
{
    char buff[BUFFER_MAXSIZE];
    memset(buff, 0, BUFFER_MAXSIZE);
    int len = read(clntSock, buff, BUFFER_MAXSIZE-1);
    if (len < 0) {
        _DEBUG("Failed to read from client");
        close(clntSock);
        return;
    }

    HttpRequest request(buff);
    HttpResponse response = router.get_http_response(request);

    std::string resp = "HTTP/1.1 200 OK\r\n"
                       "Content-Type: text/html\r\n\r\n";
    resp += response.content + "\r\n\r\n";
    if (::send(clntSock, resp.c_str(), resp.length(), 0) < 0)
        _DEBUG("Failed to send response on socket: ", clntSock);
    close(clntSock);
}

static void worker(int clntSock, Router router, std::string strRequest)
{
    _DEBUG("Worker thread invoked for socket ", clntSock);
    HttpRequest request(strRequest);
    HttpResponse response = router.get_http_response(request);

    std::string resp = "HTTP/1.1 200 OK\r\n"
                       "Content-Type: text/html\r\n\r\n";
    resp += response.content + "\r\n\r\n";

    if (send(clntSock, resp.c_str(), resp.length(), 0) < 0) {
        _DEBUG("Failed to send response on socket: ", clntSock);
    }

    /* It is assumed that at this point the server has done its job,
     * so it is closing the client socket */
    _DEBUG("Worker thread has done its job, closing socket ", clntSock);
    close(clntSock);
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
        _DEBUG("From socket ", clntSock, " recv returns value: ", rc);

        if (rc > 0) {
            /* recv() returns > 0, some data is read */
            // _DEBUG("Appending contents: ", std::string(std::begin(buff), std::begin(buff) + rc - 1));
            contents += std::string(std::begin(buff), std::begin(buff) + rc - 1);
            continue; /* continue recv-ing till all data is read */
        }
        else if (rc == 0) {
            /* recv() returns 0, client has closed this connection */
            _DEBUG("Client has closed connection on socket ", clntSock);
            ret = CLOSE_SOCK;
            break;
        }
        else if (rc < 0) {
            /* recv() returns < 0, error unless it's EWOULDBLOCK */
            if (errno != EWOULDBLOCK) {
                _DEBUG("ERROR: Failed to receive on socket ", clntSock);
                ret = CLOSE_SOCK;
            }
            else {
                /* no data for now, try later... no need to close socket */
                _DEBUG("May try again later on socket ", clntSock);
            }
            break;
        }
    } /* END: while(1) */

    /* At this point, if there is no need to close socket, 
     * there might be data for us to process, start a new
     * thread to do the job if necessary */
    if (ret != CLOSE_SOCK && contents.size() > 0) {
        std::thread t(worker, clntSock, this->router, contents);
        auto id = t.get_id();
        t.detach();
        _DEBUG("Created and detached new thread ", id, " for socket ", clntSock);

        /* a new thread is started to handle the request, 
         * it will eventually CLOSE the client socket, so
         * we must inform the server process to update the
         * fd_set so it won't need to select() on it */
        ret = CLR_FDSET;
    }

    return ret;
}

void App::run_accept(int port)
{
    _DEBUG("Invoked for port: ", port);

    int servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (servSock < 0)
        error_exit("Failed to create server socket");

    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);
    if (::bind(servSock, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)
        error_exit("Failed to bind server address");

    if (listen(servSock, 5) < 0)
        error_exit("Failed to listen");

    for (;;) {
        struct sockaddr_in clntAddr;
        unsigned int clntLen;
        int clntSock = ::accept(servSock, (struct sockaddr*)&clntAddr, &clntLen);
        if (clntSock < 0)
            error_exit("Failed to accept");

        _DEBUG("Call handling request for socket: ", clntSock);
        this->handle_request(clntSock);
    }
}

void App::run(int port)
{
    _DEBUG("Invoked for port: ", port);

    if (this->servSock >= 0) {
        _DEBUG("Already running on socket: ", this->servSock);
        return;
    }

    /* open socket to listen to client connections */
    const int servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (servSock < 0)
        error_exit("Failed to create server socket");
    this->servSock = servSock;
    _DEBUG("Created server socket: ", servSock);

    /* set socket to be reuseable */
    int reuse = 1;
    if (setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse)) < 0)
        error_exit("Failed to set socket reuseable", servSock);

    /* set socket to non-blocking, all sockets for incoming connections
     * will also be non-blocking */
    int nbio = 1;
    if (ioctl(servSock, FIONBIO, (char*)&nbio) < 0)
        error_exit("Failed to set socket non-blocking", servSock);

    /* bind the socket */
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);
    if (::bind(servSock, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)
        error_exit("Failed to bind server address", servSock);

    /* listen */
    if (listen(servSock, 5) < 0)
        error_exit("Failed to listen", servSock);

    /* initialize fd_set for select() */
    struct fd_set allSocks;
    FD_ZERO(&allSocks);
    FD_SET(servSock, &allSocks);
    int maxSock = servSock;

    /* initialize timeout interval for select() */
    struct timeval timeout;
    timeout.tv_sec = 1; /* check select() every 1 second */
    timeout.tv_usec = 0;

    int heartBeat = 1;
    for (;;) {

        /* use a temporary fd_set for pulling from select() */
        struct fd_set tmpSocks;
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
        _DEBUG("Number of sockets readable: ", nrSocks);
        for (int sd = 0; sd <= maxSock && nrSocks > 0; ++sd) {
            if (!FD_ISSET(sd, &tmpSocks))
                continue;

            /* found another readable socket */
            nrSocks--;

            if (sd == servSock) {
                _DEBUG("Server socket readable");
                int clntSock;
                do {
                    clntSock = accept(servSock, NULL, NULL);
                    if (clntSock < 0) {
                        if (errno != EWOULDBLOCK)
                            error_exit("Failed to accept", servSock);
                        break;
                    }

                    /* a new client connection accepted, add it to fd_set */
                    _DEBUG("New client connection: ", clntSock);
                    FD_SET(clntSock, &allSocks);
                    if (clntSock > maxSock)
                        maxSock = clntSock;
                } while(clntSock >= 0);
            } /* END: if servSock readable */
            else {
                _DEBUG("Client socket ", sd, " readable");
                int op = this->handle_request(sd);
                if (op != NO_CHANGE) {
                    if (op == CLOSE_SOCK) {
                        _DEBUG("Closing socket ", sd);
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

    _DEBUG("Terminating server, closing socket ", servSock);
    close(servSock);
}
