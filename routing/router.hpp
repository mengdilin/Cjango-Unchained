#ifndef _ROUTER_HPP
#define _ROUTER_HPP

#include <stdio.h>
#include <functional>
#include <unordered_map>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>

namespace cjango {
  const std::string INVALIDURL = "__INVALIDURL";
}

#ifdef DYNLOAD_CJANGO
#include <memory>
#include <dlfcn.h>
// FIXME not optimal
// class View {
// public:
//   virtual ~View() {};
//   virtual void callback() const = 0; // pure virtual function
// };

// typedef std::shared_ptr<View> HttpResponseCreateFunc();

#endif

// FIXME mock
class HttpRequest {
public:
  int some_data_member;
  std::string data;
  std::string path; // # "/music/bands/the_beatles/"
  HttpRequest(std::string str) : data(str) { path = "/abc"; }
  // HttpRequest(std::string path) : some_data_member(3), path(path) { };
};
class HttpResponse {
public:
    std::string content;
    HttpResponse(std::string str) : content(str) {}
    HttpResponse() {}
};

typedef std::function<HttpResponse(HttpRequest)> functor;
typedef std::unordered_map<std::string, functor> URLmap;

class Router {
  public:
    URLmap pattern_to_callback;
    std::vector<std::string> patterns_list;
    Router(URLmap routes) {
      for (std::pair<std::string, functor> p : routes) {
        add_route(p.first, p.second);
      }
      // "calls add_route to populate pattern_to_callback and patterns_list"
    };
    Router() {};
    void add_route(std::string url_pattern, functor f);
    // Note: register() should be renamed from add_route() for django mimicking,
    // but "register" is a reserved word in C++.
    std::string resolve(HttpRequest);
    HttpResponse get_http_response(HttpRequest);
};

#endif
