#include "router.hpp"
#include "../app/externs.hpp"

inline bool registered(std::vector<std::string> patterns_list, std::string url_pattern) {
  bool found = find(patterns_list.begin(),
                    patterns_list.end(), url_pattern) != patterns_list.end();
  return found;
}

void Router::add_route(std::string url_pattern, functor f) {
  // Note: url_pattern may be a regex pattern or an actual url_path
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
    // exit(EXIT_FAILURE);
  }
  return lib;
}
void *load_callback(void *lib, const std::string& func_name) {
  const auto func = dlsym(lib, func_name.c_str());
  const auto dlsym_error = dlerror();
  if (dlsym_error) {
    _DEBUG("Cannot load symbol callback()");
    dlclose(lib);
    // exit(EXIT_FAILURE);
  }
  return func;
}
#endif

HttpResponse Router::get_http_response(HttpRequest request) {
  std::string url_path = resolve(request);
  if (url_path == cjango::INVALIDURL) {
    _DEBUG("Router::get_http_response(): this HttpRequest.path is invalid");
    return HttpResponse();
  }

#ifdef DYNLOAD_CJANGO
  // FIXME read shared object names from config file
  const auto mylib = load_shared_object_file("callbacks/mycallback.so");
  // Note: the file path is from an executable, not from this source file

  //const auto get_view = static_cast<HttpResponseCreateFunc*>(load_callback(mylib, "get_view"));
  //const auto& view_class = get_view();
  typedef HttpResponse (*callback_t)(HttpRequest);
   // FIXME STUPIDLY UGLY
  HttpResponse (*myfun)(HttpRequest) =
    (HttpResponse (*)(HttpRequest)) load_callback(mylib, "callback_mine");
  // return view_class->callback(request);
  return (*myfun)(request);
#else
  functor callback = pattern_to_callback[url_path];
  _DEBUG("Router::get_http_response(): return callback");
  return callback(request);
#endif
}