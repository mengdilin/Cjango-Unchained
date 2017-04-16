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

#ifdef CJANGO_DYNLOAD
#include <memory>
#include <dlfcn.h>

using dlib_handler = void *;
#endif

// FIXME scoped enum
namespace cjango {
  const std::string INVALIDURL = "__INVALIDURL";
  const std::string STATIC_FILE_SERVED = "__STATIC_FILE_SERVED";
}

using functor = std::function<http::HttpResponse(http::HttpRequest)>;
using URLmap = std::unordered_map<std::string, functor>;

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
#ifdef CJANGO_DYNLOAD
    std::unordered_map<std::string, int> ref_count;
    std::vector<dlib_handler> dlib_handlers;
    void *load_shared_object_file(const std::string& path);
    void *load_callback(const std::string& path, const std::string& func_name);
    void load_url_pattern_from_file();
#endif
    void register_static_file_routing();
    // Note: register() should be renamed from add_route() for django mimicking,
    // but "register" is a reserved word in C++.
    std::string resolve(http::HttpRequest);
    http::HttpResponse get_http_response(http::HttpRequest);
};

#endif
