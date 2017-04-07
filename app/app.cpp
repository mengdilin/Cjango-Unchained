#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "app.hpp"
#include "../http_parser/http_request_parser.hpp"
#include <sstream>
#include <unordered_map>
#include <FileWatcher.h>

using namespace std;

#define BUFFER_MAXSIZE 1024

void App::print_routes()
{
    cout << "Hello app!" << endl;
}

void error_exit(string msg)
{
    // _DEBUG(msg);
    perror(msg.c_str());
    exit(1);
}

void App::worker(int clntSock)
{
    char buff[BUFFER_MAXSIZE];
    memset(buff, 0, BUFFER_MAXSIZE);
    int len = read(clntSock, buff, BUFFER_MAXSIZE-1);
    if (len < 0) {
        _DEBUG("Failed to read from client");
        close(clntSock);
        return;
    }
    // _DEBUG("Received request from client:\n", string(buff));

    HttpRequest request(buff);
    try {
        http::HttpRequestParser parser;
        std::stringstream ss;
        ss << buff;
        //std::cout << ss << std::endl;
        http::HttpRequest headers = parser.parse_request_line_and_headers(ss);
        std::unordered_map<std::string, std::string> map = parser.parse_body(ss, "application/x-www-form-urlencoded", ss.str().length());

        //std::unordered_map<std::string, std::string> body = parser.parse_body(ss, );
        _DEBUG(headers); // FIXME _DEBUG for multi lines
        _DEBUG("finished headers");
        for (auto entry : map) {
            _DEBUG(entry.first, ":", entry.second);
        }
    } catch (const char *e) {
    // if you throw a char* instead of exception class, that comes as const char*
        std::cout << e << std::endl;
    }
    request.path = "/abc"; // FIXME set path
    // HttpResponse response = get_phony_response(request);
    HttpResponse response = router.get_http_response(request);

    string resp = "HTTP/1.1 200 OK\r\n"
                  "Content-Type: text/html\r\n\r\n";
    resp += response.content + "\r\n\r\n";
    // _DEBUG("Response:\n", resp);
    // if (write(clntSock, resp.c_str(), resp.length()) < 0)
    if (::send(clntSock, resp.c_str(), resp.length(), 0) < 0)
        _DEBUG("Failed to send response on socket: ", clntSock);
    close(clntSock);
}

void App::handle_request(int clntSock)
{
    thread t(&App::worker, this, clntSock);
    auto id = t.get_id();
    t.detach();
    _DEBUG("Created and detached new thread ", id, " for socket ", clntSock);
}

#ifdef DYNLOAD_CJANGO
#include <chrono>
void App::monitor_file_change() {
    for (;;) {
        this->fileWatcher.update(is_file_updated);
        if(is_file_updated) {
            router.load_url_pattern_from_file();
            _DEBUG("url-pattern.json is updated\n\n");
            is_file_updated = false;
        }
        // prevent busy looping -- should take a rest for a while
        // This would reduce CPU usage of ./testrun from 100% to 0.1%
        std::this_thread::sleep_for(1s);
    }
}
#endif

void App::run(int port)
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

#ifdef DYNLOAD_CJANGO
    // run a thread for checking file changes by every 1 second independently
    thread t(&App::monitor_file_change, this);
    auto monitor_id = t.get_id();
    t.detach();
    _DEBUG("Created and detached new thread with monitor_id: ", monitor_id);
#endif

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
