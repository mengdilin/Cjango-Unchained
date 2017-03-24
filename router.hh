#include <stdio.h>
#include <functional>
#include <map>
#include <vector>
#include <string>

// FIXME mock
class HttpRequest {
public:
  int some_data_member;
  HttpRequest() : some_data_member(3) {};
};
class HttpResponse {
public:
  int some_data_member;
  HttpResponse() : some_data_member(3) {};
};

typedef std::function<HttpResponse(HttpRequest)> functor;
typedef std::map<std::string, functor> URLmap;


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
    void add_route(std::string url_pattern, functor f);
    // Note: register() should be renamed from add_route() for django mimicking,
    // but "register" is a reserved word in C++.
    std::string resolve(HttpRequest);
    HttpResponse get_http_response(HttpRequest);
};