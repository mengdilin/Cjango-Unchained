#ifndef _APP_HPP
#define _APP_HPP

#include <string>
#include "externs.hpp"

using namespace std;

class App {
    Router router;
public:
    App() {}
    App(Router& rt): router(rt) {}
    void print_routes();
    void run(int port);
    void handle_request(int socket);
};

#endif