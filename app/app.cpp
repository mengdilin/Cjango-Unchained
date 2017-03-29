#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "app.hpp"

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

static void worker(int clntSock, Router router)
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
    thread t(worker, clntSock, this->router);
    auto id = t.get_id();
    t.detach();
    _DEBUG("Created and detached new thread ", id, " for socket ", clntSock);
}

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