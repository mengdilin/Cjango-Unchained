#include "router.hpp"
#include <regex>
#include "../app/externs.hpp"
#ifdef CJANGO_DYNLOAD
#include <algorithm>
#endif

std::string g_callbacks_root_dir;
std::string g_url_json_dir;

std::string Router::static_root_dir; // static var has to be defined in cpp

inline bool registered(std::vector<std::string> plist, std::string url_pattern) {
  bool found = find(plist.begin(), plist.end(), url_pattern) != plist.end();
  return found;
}

/**
** @brief Add a mapping from url_pattern to a callback into patterns_list and pattern_to_callback.
** if the given url pattern is already set, this will overwrite the pattern by the new callback.
**
** @param url_pattern (e.g. "/diary/[0-9]{4}")
** @return void
*/
void Router::add_route(std::string url_pattern, functor f) {
#ifdef CJANGO_DYNLOAD

  if (! registered(patterns_list, url_pattern))
    patterns_list.push_back(url_pattern);
  pattern_to_callback[url_pattern] = f;
  _SPDLOG(route_logger_name, info, "updated route: {}", url_pattern);

#else

  if (registered(patterns_list, url_pattern)) {
    _SPDLOG(route_logger_name, warn, "catched: url pattern is already registered");
    _SPDLOG(route_logger_name, info, "rule '{}' is overwritten by a new callback", url_pattern);
  } else {
    patterns_list.push_back(url_pattern);
    _SPDLOG(route_logger_name, info, "added a new rule: {}", url_pattern);
  }
  pattern_to_callback[url_pattern] = f;

#endif
}

/**
** @brief Called in get_http_response() for mapping a given request's path to an url pattern.
** If no pattern matches the given request's path, RouterException::INVALID_URL will be returned.
** If starting from "/static/", RouterException::STATIC_FILE_SERVED will be returned.
** Both enums are handled in get_http_response().
** @return a corresponding url pattern such as "diary/[0-9]{4}/[0-9]{2}/"
*/
std::string Router::resolve(http::HttpRequest request) const {

  std::string path = request.get_path();
  for (const auto &p : patterns_list) {
    std::regex rgx(p);
    if (std::regex_match(path, rgx))
      return p;
  }

  std::regex rgx(get_static_dir() + "/.*"); // FIXME arbitrary static folder
  if (std::regex_match(path, rgx)) {
    throw RouterException::STATIC_FILE_SERVED;
  }

  _SPDLOG(route_logger_name, error,
          "resolve(): this path doesn't match any rules: {}",
          request.get_path());

  throw RouterException::INVALID_URL;
}

#ifdef CJANGO_DYNLOAD
void *Router::load_shared_object_file(const std::string& path) {
  const auto lib = dlopen((g_callbacks_root_dir + path).c_str(), RTLD_LAZY);
  if (!lib) {
    // Note: two successive dlerror() calls result in segfault
    const auto errmsg = dlerror();
    _SPDLOG(route_logger_name, error, "Cannot load library: {}", errmsg);
    throw "no such an object file";
    // exit(EXIT_FAILURE);
  }
  bool found = ref_count.find(path) != ref_count.end();
  if (found)
    ++ref_count[path];
  else
    ref_count[path] = 1;
  return lib;
}

callback_type Router::load_callback(const std::string& path, const std::string& func_name) {
  auto lib = (dlib_handler) load_shared_object_file(path);
  _SPDLOG(route_logger_name, debug, "dlopen() finished for {}", path);
  bool found = std::find(dlib_handlers.begin(), dlib_handlers.end(), lib) != dlib_handlers.end();
  if (found) {
    // MAYBE-LATER RTLD_NOLOAD would be much cleaner
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
  _SPDLOG(route_logger_name, debug,
          "dlib_handlers.size: {} ref_count: {}", dlib_handlers.size(), ref_count[path]);
  const auto func = dlsym(lib, func_name.c_str());
  const auto dlsym_error = dlerror();
  if (dlsym_error) {
    _SPDLOG(route_logger_name, error, "Cannot load symbol {}", dlsym_error);
    throw "no such an object file";
  }
   // FIXME where should we invoke dlclise()?
  return reinterpret_cast<callback_type>(func);
}

#include "../lib/json.hpp"
#include <fstream>
// third-party library for json parsing. Usage: just loading this file
// usage: github nlohmann/json

/**
** @brief called in App::monitor_file_change() for reloading entire url mappings
*/
void Router::load_url_pattern_from_file(const std::string url_json_dir) {

  erase_all_patterns();

  std::ifstream i(url_json_dir + "urls.json");
  printf("%s   \n", url_json_dir.c_str());
  nlohmann::json j;
  i >> j;
  _SPDLOG(route_logger_name, info, "loaded urls.json");
  _SPDLOG(route_logger_name, info, "callback dir: {}", g_callbacks_root_dir);
  for (nlohmann::json::iterator it = j.begin(); it != j.end(); ++it) {
    const auto cinfo = it.value(); // callback info


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
      callback = static_cast<functor>(load_callback(cinfo["file"], cinfo["funcname"]));
    } catch (const char *e) {
      callback = [](http::HttpRequest h){ return http::HttpResponse("<h1>500 Internal Server Error</h1>"); };
      _SPDLOG(route_logger_name, info, "Invalid callback");
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

  std::string url_path;
  try {
    url_path = resolve(request);
  } catch (RouterException err) {
    switch(err) {
      case RouterException::INVALID_URL:
        _SPDLOG(route_logger_name, warn, "this HttpRequest.path is invalid: {}", request.get_path());
        return http::HttpResponse("Cjango: 404 Page Not Found");
      case RouterException::STATIC_FILE_SERVED:
        const std::regex r("((png|gif|jpeg|bmp|webp))$");
        std::smatch sm;
        // std::regex_search forbids a temporary string
        std::string path = request.get_path();
        std::string suffix;
        if (std::regex_search(path, sm, r))
          suffix = sm[0];
        // MAYBE-LATER other than image
        _SPDLOG(route_logger_name, info, "imageSuffix: {}", suffix);
        return http::HttpResponse::render_to_response(".." + request.get_path(), "image/" + suffix, request);
    }
  }

  functor callback = pattern_to_callback[url_path];
  _SPDLOG(route_logger_name, info, "return callback for: {}", url_path);
  return callback(request);
}
