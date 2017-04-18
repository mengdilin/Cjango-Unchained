#include "router.hpp"
#include <regex>
#include "../app/externs.hpp"

inline bool registered(std::vector<std::string> patterns_list, std::string url_pattern) {
  bool found = find(patterns_list.begin(),
                    patterns_list.end(), url_pattern) != patterns_list.end();
  return found;
}

/**
** @brief add a mapping from url_pattern to a callback function into patterns_list and pattern_to_callback
** @param url_pattern
** @return void
*/
void Router::add_route(std::string url_pattern, functor f) {
  // Note: url_pattern may be a regex pattern or an actual url_path
#ifdef CJANGO_DYNLOAD
#include <algorithm>

  if (! registered(patterns_list, url_pattern))
    patterns_list.push_back(url_pattern);
  pattern_to_callback[url_pattern] = f;
  _SPDLOG(cjango::route_logger_name, info, "updated route: {}", url_pattern);

#else
  try {
    if (registered(patterns_list, url_pattern))
      throw 1;
  }
  catch (int e) {
    _SPDLOG(cjango::route_logger_name, warn, "catched: url pattern is already registered");
    return;
  }
  patterns_list.push_back(url_pattern);
  pattern_to_callback[url_pattern] = f;
  _SPDLOG(cjango::route_logger_name, info, "added a new rule: {}", url_pattern);
#endif
}

/**
** @brief called in get_http_response() for mapping a given request's path to a corresponding url pattern
** @return a corresponding url pattern such as "diary/[0-9]{4}/[0-9]{2}/"
*/
std::string Router::resolve(http::HttpRequest request) {

  std::string url_pattern = request.get_path();
  for (const auto &p : patterns_list) {
    std::regex r(p);
    if (std::regex_match(url_pattern, r))
      return p;
  }

  std::regex r("/static/.*");
  if (std::regex_match(url_pattern, r)) {
    return cjango::STATIC_FILE_SERVED;
  }

  _SPDLOG(cjango::route_logger_name, error,
          "resolve(): this url_pattern isn't registered: {}", request.get_path());
  return cjango::INVALIDURL;
  // "resolver_match: a resolved url. This attribute is only set after URL resolving took place"
}

#ifdef CJANGO_DYNLOAD
void *Router::load_shared_object_file(const std::string& path) {
  const auto lib = dlopen(path.c_str(), RTLD_LAZY);
  if (!lib) {
    // Note: two successive dlerror() calls result in segfault
    const auto human_readable_str = dlerror();
    _SPDLOG(cjango::route_logger_name, error, "Cannot load library: {}", human_readable_str);
    throw "no such a object file";
    // exit(EXIT_FAILURE);
  }
  bool found = ref_count.find(path) != ref_count.end();
  if (found)
    ++ref_count[path];
  else
    ref_count[path] = 1;
  return lib;
}

void *Router::load_callback(const std::string& path, const std::string& func_name) {
  auto lib = (dlib_handler) load_shared_object_file(path);
  _SPDLOG(cjango::route_logger_name, debug, "dlopen() finished for {}", path);
  bool found = std::find(dlib_handlers.begin(), dlib_handlers.end(), lib) != dlib_handlers.end();
  if (found) {
    // dlopen() returns the same (opaque) handler when specified twice
    // For updating the same-name function, we first close (as the same times as dlopen()),
    // unload the library, and then reload the updated function
    dlib_handlers.erase(std::remove(dlib_handlers.begin(), dlib_handlers.end(), lib),
                        dlib_handlers.end());
    dlclose(lib); dlclose(lib);
    ref_count[path] = ref_count[path] - 2;
    // It looks strange but this is the right way to detect same file handlers are returned
    lib = (dlib_handler) load_shared_object_file(path);
  }
  dlib_handlers.push_back(lib);
  _SPDLOG(cjango::route_logger_name, debug,
          "dlib_handlers.size: {} ref_count: {}", dlib_handlers.size(), ref_count[path]);

  const auto func = dlsym(lib, func_name.c_str());
  const auto dlsym_error = dlerror();
  if (dlsym_error) {
    _SPDLOG(cjango::route_logger_name, error, "Cannot load symbol {}", dlsym_error);
    throw "no such a callback name";
    // exit(EXIT_FAILURE);
  }
   // FIXME where should we invoke dlclise()?
  return func;
}

#include "../lib/json.hpp"
#include <fstream>
// third-party library for json parsing. Usage: just loading this file
// usage: github nlohmann/json

/**
** @brief called in App::monitor_file_change() for reloading entire url mappings
*/
void Router::load_url_pattern_from_file() {
  std::ifstream i("../json/urls.json");
  nlohmann::json j;
  i >> j;
  _SPDLOG(cjango::route_logger_name, info, "loaded urls.json");
  for (nlohmann::json::iterator it = j.begin(); it != j.end(); ++it) {
    const auto cinfo = it.value(); // callback info

    using callback_type = http::HttpResponse (*)(http::HttpRequest);
    // A pointer type of a function
    // receiving http::HttpRequest as an argument and returns http::HttpResponse
    // Item 9 in "Effective Modern C++" prefers alias (using) to typedef
    // because of readability and easiness to use with template
    functor callback;

    try {
      // Aviding "no matching conversion for C-style cast from 'void *' to
      // 'functor' (aka 'function<http::HttpResponse (http::HttpRequest)>')" error,
      // here we first cast to callback_type, and then the callback_type is
      // implicitly casted into std::function<callback_type>
      callback = (callback_type) load_callback(cinfo["file"], cinfo["funcname"]);
    } catch (const char *e) {
      callback = [](http::HttpRequest h){ return http::HttpResponse("invalid callback specified"); };
      _SPDLOG(cjango::route_logger_name, info, "Invalid callback: {} {}", cinfo["file"], cinfo["funcname"]);
      // continue;
    }
    add_route(it.key(), callback);
    // cannot dlclose here because callbacks will be used later
  }
}
#endif

/**
** @brief find and execute a callback corresponding to the given request
** @return an HttpResponse object built by the callback
*/
http::HttpResponse Router::get_http_response(http::HttpRequest request) {
  std::string url_path = resolve(request);
  if (url_path == cjango::INVALIDURL) {
    _SPDLOG(cjango::route_logger_name, warn, "this HttpRequest.path is invalid: {}", request.get_path());
    return http::HttpResponse("Cjango: 404 Page Not Found");
  } else if (url_path == cjango::STATIC_FILE_SERVED) {
    const std::regex r("((png|gif|jpeg|bmp|webp))$");
    std::smatch sm;
    // std::regex_search forbids a temporary string
    std::string path = request.get_path();
    std::string suffix;
    if (std::regex_search(path, sm, r))
      suffix = sm[0];
    // MAYBE-LATER other than image
    _SPDLOG(cjango::route_logger_name, info, "imageSuffix: {}", suffix);
    return http::HttpResponse::render_to_response("callbacks" + request.get_path(), "image/" + suffix, request);
  }

  functor callback = pattern_to_callback[url_path];
  _SPDLOG(cjango::route_logger_name, info, "return callback for: ", url_path);
  return callback(request);
}