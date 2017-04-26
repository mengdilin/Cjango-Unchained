#ifndef _MSOCKET_HPP
#define _MSOCKET_HPP

#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <errno.h>
#include "externs.hpp"

// constexpr std::string logmskt = "skt";

class MSocket {
public:
    MSocket();
    MSocket(int sock);
    ~MSocket();

    int socket() { return msock; }
    void close();
    int set_reusable();
    int set_nonblocking();
    int bind(int port);
    int listen(int maxconn);
    std::shared_ptr<MSocket> accept();
    int recv(void* buf, size_t len);
    int send(void* buf, size_t len);
    int send(std::string str);

private:
    int msock = -1;
    bool sock_closed = false;
};
#if 0
MSocket::MSocket() {
    _SPDLOG(logmskt, info, "Constructor called for MSocket");
    int rc = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (rc < 0) {
        _SPDLOG(logmskt, info, "Failed to create socket");
        throw std::string("Failed to create socket");
    }

    msock = rc;
    _SPDLOG(logmskt, info, "Created socket {}", msock);
}

MSocket::MSocket(int sock) {
    _SPDLOG(logmskt, info, "Constructor called for MSocket with arg {}", sock);
    if (sock) {
        msock = sock;
        _SPDLOG(logmskt, info, "Assigned socket {}", msock);
    }
}

MSocket::~MSocket() {
    _SPDLOG(logmskt, info, "Destructor called for MSocket {}", msock);
    if (msock > 0 && !sock_closed)
        ::close(msock);
}

void MSocket::close() {
    _SPDLOG(logmskt, info, "Closing socket {} ({})", msock, sock_closed?"abnormal":"normal");
    if (msock > 0 && !sock_closed) {
        ::close(msock);
        sock_closed = true;
    }
}

int MSocket::set_reusable() {
    if (msock <= 0)
        throw std::string("Attempt to operate on uncreated socket");

    int reuse = 1;
    int rc = setsockopt(msock, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse));
    if (rc < 0)
        throw std::string("Failed to set socket");
    return rc;
}

int MSocket::set_nonblocking() {
    if (msock <= 0)
        throw std::string("Attempt to operate on uncreated socket");

    int fl = fcntl(msock, F_GETFL);
    if (fl < 0)
        throw std::string("Failed to get socket flags");

    fl = fcntl(msock, F_SETFL, fl | O_NONBLOCK);
    if (fl < 0)
        throw std::string("Failed to set socket flags");
    return fl;
}

int MSocket::bind(int port) {
    if (msock <= 0)
        throw std::string("Attempt to operate on uncreated socket");

    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);
    int rc = ::bind(msock, (struct sockaddr*)&servAddr, sizeof(servAddr));
    if (rc < 0)
        throw std::string("Failed to bind");
    return rc;
}

int MSocket::listen(int maxconn) {
    if (msock <= 0)
        throw std::string("Attempt to operate on uncreated socket");

    int rc = ::listen(msock, maxconn);
    if (rc < 0)
        throw std::string("Failed to listen");
    return rc;
}

std::shared_ptr<MSocket> MSocket::accept() {
    if (msock <= 0)
        throw std::string("Attempt to operate on uncreated socket");

    int rc = ::accept(msock, NULL, NULL);
    if (rc < 0) {
        if (errno != EWOULDBLOCK)
            throw std::string("Failed to accept");
        return NULL;
    }

    return std::shared_ptr<MSocket>(new MSocket(rc));
}

int MSocket::recv(void* buf, size_t len) {
    if (msock <= 0)
        throw std::string("Attempt to operate on uncreated socket");

    int rc = ::recv(msock, buf, len, 0);
    if (rc == 0)
        throw std::string("Failed to recv: closed by remote");
    if (rc < 0 && errno != EWOULDBLOCK)
        throw std::string("Failed to recv");
    return rc;
}

int MSocket::send(void* buf, size_t len) {
    if (msock <= 0)
        throw std::string("Attempt to operate on uncreated socket");

    int rc = ::send(msock, buf, len, 0);
    if (rc < 0)
        throw std::string("Failed to send");
    return rc;
}

int MSocket::send(std::string str) {
    if (msock <= 0)
        throw std::string("Attempt to operate on uncreated socket");

    const char* buff = str.c_str();
    unsigned int length = str.length();
    unsigned int total_sent = 0;
    unsigned int left_to_send = length;
    int rc = 0;
    while(total_sent < length)
    {
        rc = ::send(msock, buff + total_sent, left_to_send, 0);
        if(rc < 0)
            throw std::string("Failed to send");
        else {
            total_sent += rc;
            left_to_send -= rc;
        }
    }
    return rc;
}
#endif
#endif
