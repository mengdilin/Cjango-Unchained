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
using callback_type = http::HttpResponse (*)(http::HttpRequest);
#endif

/**
* @file  router.hpp
* @brief Router class definition and related consts/aliases
* @author   caprice-j
* @date   2017.04.17
* @version   0.1
* @warning  this example may include some wrong descriptions
*/

// FIXME scoped enum
namespace cjango {
  const std::string INVALIDURL = "__INVALIDURL";
  const std::string STATIC_FILE_SERVED = "__STATIC_FILE_SERVED";
  const std::string route_logger_name = "route";
}

using functor = std::function<http::HttpResponse(http::HttpRequest)>;
using URLmap = std::unordered_map<std::string, functor>;

class Router {
  private:
    URLmap pattern_to_callback;
    std::vector<std::string> patterns_list;
    std::string static_root_dir;
#ifdef CJANGO_DYNLOAD
    std::vector<dlib_handler> dlib_handlers;
    std::unordered_map<std::string, int> ref_count;
#endif
  public:
    Router(URLmap routes) {
      for (std::pair<std::string, functor> p : routes) {
        add_route(p.first, p.second);
      }
      // "calls add_route to populate pattern_to_callback and patterns_list"
    };
    Router() {};
    void add_route(std::string url_pattern, functor f);
#ifdef CJANGO_DYNLOAD
    void *load_shared_object_file(const std::string& path);
    callback_type load_callback(const std::string& path, const std::string& func_name);
    void load_url_pattern_from_file();
#endif
    void set_static_root_dir(std::string dir) { static_root_dir = dir; };
    std::string get_static_root_dir() { return static_root_dir; };
    // Note: register() should be renamed from add_route() for django mimicking,
    // but "register" is a reserved word in C++.
    std::string resolve(http::HttpRequest);
    http::HttpResponse get_http_response(http::HttpRequest);
};

#endif
