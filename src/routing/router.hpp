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

enum class RouterException { INVALID_URL, STATIC_FILE_SERVED };
const std::string route_logger_name = "route"; //FIXME non-global

using functor = std::function<http::HttpResponse(http::HttpRequest)>;
using URLmap = std::unordered_map<std::string, functor>;

class Router {
  private:
    URLmap pattern_to_callback;
    std::vector<std::string> patterns_list;
    static std::string static_root_dir; // class variable
#ifdef CJANGO_DYNLOAD
    std::vector<dlib_handler> dlib_handlers;
    std::unordered_map<std::string, int> ref_count;
#endif
  public:
    Router(URLmap routes) {
      for (std::pair<std::string, functor> p : routes)
        add_route(p.first, p.second);
    };
    Router() {};
    int nr_patterns() const { return patterns_list.size(); };
    void erase_all_patterns() { patterns_list.clear(); pattern_to_callback.clear(); };
    void add_route(std::string url_pattern, functor f);
#ifdef CJANGO_DYNLOAD
    void *load_shared_object_file(const std::string& path);
    functor load_callback(const std::string& path, const std::string& func_name);
    void load_url_pattern_from_file(const std::string url_json_dir);
#endif
    void set_static_dir(const std::string dir) { static_root_dir = dir; };
    std::string get_static_dir() const { return static_root_dir; };
    // Note: register() should be renamed from add_route() for mimicking django,
    // but "register" is a reserved word in C++.
    std::string resolve(http::HttpRequest) const;
    http::HttpResponse get_http_response(http::HttpRequest);
};

#endif
