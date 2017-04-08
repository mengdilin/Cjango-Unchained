#ifndef _ROUTER_HPP
#define _ROUTER_HPP

#include <stdio.h>
#include <functional>
#include <unordered_map>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <cjango>

#ifdef DYNLOAD_CJANGO
#include <memory>
#include <dlfcn.h>
// FIXME not optimal
// class View {
// public:
//   virtual ~View() {};
//   virtual void callback() const = 0; // pure virtual function
// };

// typedef std::shared_ptr<View> http::HttpResponseCreateFunc();
using dlib_handler = void *;
#endif

namespace cjango {
  const std::string INVALIDURL = "__INVALIDURL";
}

using functor = std::function<http::HttpResponse(http::HttpRequest)>;
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
#ifdef DYNLOAD_CJANGO
    std::unordered_map<std::string, int> ref_count;
    std::vector<dlib_handler> dlib_handlers;
    void *load_shared_object_file(const std::string& path);
    void *load_callback(const std::string& path, const std::string& func_name);
    void load_url_pattern_from_file();
#endif
    // Note: register() should be renamed from add_route() for django mimicking,
    // but "register" is a reserved word in C++.
    std::string resolve(http::HttpRequest);
    http::HttpResponse get_http_response(http::HttpRequest);
};

#endif
