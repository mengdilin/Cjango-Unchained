#ifndef _APP_HPP
#define _APP_HPP

#include <string>
#include <cstring>
#include "externs.hpp"

using namespace std;

class App {
public:
    Router router; // FIXME how router can be private?
    void add_route(std::string url_pattern, functor f) {
      router.add_route(url_pattern, f);
    }
    App() {}
    // App(Router& rt): router(rt) {}
    void worker(int clntSock);
    void print_routes();
    void run(int port);
    void handle_request(int socket);
};

#endif
