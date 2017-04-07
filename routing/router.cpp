#include "router.hpp"
#include "../app/externs.hpp"

inline bool registered(std::vector<std::string> patterns_list, std::string url_pattern) {
  bool found = find(patterns_list.begin(),
                    patterns_list.end(), url_pattern) != patterns_list.end();
  return found;
}

void Router::add_route(std::string url_pattern, functor f) {
  // Note: url_pattern may be a regex pattern or an actual url_path
#ifdef DYNLOAD_CJANGO

  if (! registered(patterns_list, url_pattern))
    patterns_list.push_back(url_pattern);
  pattern_to_callback[url_pattern] = f;
  std::cout << "updated route: " << url_pattern << std::endl;

#else
  try {
    if (registered(patterns_list, url_pattern))
      throw 1;
  }
  catch (int e) {
    _DEBUG("catched: url pattern is already registered");
    return;
  }
  patterns_list.push_back(url_pattern);
  pattern_to_callback[url_pattern] = f;
#endif
  _DEBUG("add a new rule: ", url_pattern.c_str());
}

std::string Router::resolve(HttpRequest request) {
  // return its corresponding url_pattern
  // MAYBE-LATER map request.path -> url_pattern
  std::string url_pattern = request.path;
  if (registered(patterns_list, url_pattern)) {
    _DEBUG("resolve(): registered");
    return url_pattern;
  } else {
    _DEBUG("resolve(): non registered");
    return cjango::INVALIDURL;
  }
  // "resolver_match: a resolved url. This attribute is only set after URL resolving took place"
}

#ifdef DYNLOAD_CJANGO
void *load_shared_object_file(const std::string& path) {
  const auto lib = dlopen(path.c_str(), RTLD_LAZY);
  if (!lib) {
    _DEBUG("Cannot load library: ", dlerror());
    std::cout <<  "Cannot load library: " << dlerror() << std::endl;
    throw "no such a object file";
    // exit(EXIT_FAILURE);
  }
  return lib;
}
void *load_callback(void *lib, const std::string& func_name) {
  const auto func = dlsym(lib, func_name.c_str());
  const auto dlsym_error = dlerror();
  if (dlsym_error) {
    _DEBUG("Cannot load symbol callback()");
    std::cout <<  "Cannot load symbol: " << dlerror() << std::endl;
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
void Router::load_url_pattern_from_file() {
  std::ifstream i("callbacks/url-pattern.json");
  nlohmann::json j;
  i >> j;
  _DEBUG("loaded url-pattern.json");
  for (nlohmann::json::iterator it = j.begin(); it != j.end(); ++it) {
    const auto cinfo = it.value(); // callback info

    HttpResponse (*myfun)(HttpRequest); // function pointer

    try {
      auto mylib = load_shared_object_file(cinfo["file"]);
      myfun =
        (HttpResponse (*)(HttpRequest)) load_callback(mylib, cinfo["funcname"]);
    } catch (const char *e) {
      _DEBUG("aaaa\n\n\n\n\n\n\n\n");
      continue;
    }
    functor callback = myfun;
    _DEBUG(it.key(), " : ", it.value(), "\n",
           "cinfo['file']", cinfo["file"], "cinfo['funcname']", cinfo["funcname"]);
    add_route(it.key(), callback);
    // cannot dlclose here because callbacks will be used later
  }
}
#endif

HttpResponse Router::get_http_response(HttpRequest request) {
  std::string url_path = resolve(request);
  if (url_path == cjango::INVALIDURL) {
    _DEBUG("Router::get_http_response(): this HttpRequest.path is invalid");
    return HttpResponse();
  }

  functor callback = pattern_to_callback[url_path];
  _DEBUG("Router::get_http_response(): return callback");
  return callback(request);
}