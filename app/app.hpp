#ifndef _APP_HPP
#define _APP_HPP

#include <string>
#include <cstring>
#include "externs.hpp"

using namespace std;

class App {
    int servSock; /* server socket id */
public:
    Router router; // FIXME how router can be private?
    void add_route(std::string url_pattern, functor f) {
      router.add_route(url_pattern, f);
    }
    App(): servSock{-1} {}
    //App(Router& rt): router(rt), servSock{-1} {}
    App(URLmap routes): servSock{-1}
    {
        Router tmp(routes);
        this->router = tmp;
    }
    void worker(int clntSock, string strRequest);
    void print_routes();
    void run(int port);
    void run_accept(int port);
    int handle_request(int socket);
};

#endif
