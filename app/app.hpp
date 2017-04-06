#ifndef _APP_HPP
#define _APP_HPP

#include <string>
#include "externs.hpp"

using namespace std;

class App {
    Router router;
    int servSock; /* server socket id */
public:
    App(Router& rt): router(rt), servSock{-1} {}
    App(URLmap routes): servSock{-1}
    {
        Router tmp(routes);
        this->router = tmp;
    }

    void print_routes();
    void run(int port);
    void run_accept(int port);
    int handle_request(int socket);
};

#endif