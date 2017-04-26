#ifndef _SELECTOR_HPP
#define _SELECTOR_HPP

#include <unistd.h>
#include <string>
#include <sys/time.h>
#include "externs.hpp"

static const std::string logslct = "slct";

struct Selector {
    Selector();
    Selector(int sock);

    int servSock = -1;
    /* fd_set for users' set/clr/zero ops */
    fd_set allSocks;
    /* fd_set that stores the result from ::select() call 
     * fd_isset show read from this fd_set
     */
    fd_set retSocks;
    struct timeval timeout;
    int maxSock = -1;

    void set_server(int sock);
    void fd_zero();
    void fd_set(int sock);
    bool fd_isset(int sock, int m=0);
    void fd_clr(int sock);
    bool server_isset(int m=0);

    int select();

    void report(int m=0);
};

void Selector::set_server(int sock) {
    servSock = sock;
}

void Selector::fd_zero() {
    FD_ZERO(&allSocks);
}

void Selector::fd_set(int sock) {
    FD_SET(sock, &allSocks);
    if (sock > maxSock)
        maxSock = sock;
}

bool Selector::fd_isset(int sock, int m) {
    if (!m)
        return FD_ISSET(sock, &retSocks);
    else
        return FD_ISSET(sock,&allSocks);
}

void Selector::fd_clr(int sock) {
    FD_CLR(sock, &allSocks);
    if (sock == maxSock) {
        while (!fd_isset(maxSock))
            maxSock--;
    }
}

bool Selector::server_isset(int m) {
    if (servSock < 0)
        return false;
    return fd_isset(servSock, m);
}

int Selector::select() {
    FD_ZERO(&retSocks);
    memcpy(&retSocks, &allSocks, sizeof(allSocks));
    return ::select(maxSock + 1, &retSocks, NULL, NULL, &timeout);
}

void Selector::report(int m) {
    _SPDLOG(logslct, info, "report from selector ...");
    _SPDLOG(logslct, info, "max socket number {}", maxSock);
    for (auto i = 1; i <= maxSock; ++i) {
        if (fd_isset(i, m))
            _SPDLOG(logslct, info, "socket: {} is set in {} fd_set", i, m?"query":"return");
    }
}
 
Selector::Selector() {
    FD_ZERO(&allSocks);
    FD_ZERO(&retSocks);
    timeout.tv_sec = 1; /* check select() every 1 second */
    timeout.tv_usec = 0;
}

Selector::Selector(int sock) {
    set_server(sock);
    FD_ZERO(&allSocks);
    FD_ZERO(&retSocks);
    timeout.tv_sec = 1; /* check select() every 1 second */
    timeout.tv_usec = 0;
}

#endif