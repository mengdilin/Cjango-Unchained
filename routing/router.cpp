#include "router.hpp"
#include <regex>
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
    // _DEBUG("resolve(): non registered");
    // return cjango::INVALIDURL;
    _DEBUG("resolve(): regex");
    for (const auto &p : patterns_list) {
      std::regex r(p);
      if (std::regex_match(url_pattern, r))
        return p;
    }
  }
  _DEBUG("resolve(): non registered");
  return cjango::INVALIDURL;
  // "resolver_match: a resolved url. This attribute is only set after URL resolving took place"
}

HttpResponse Router::get_http_response(HttpRequest request) {
  std::string url_path = resolve(request);
  if (url_path == cjango::INVALIDURL) {
    _DEBUG("Router::get_http_response(): this HttpRequest.path is invalid");
    return HttpResponse();
  }

  functor callback = pattern_to_callback[url_path];
  _DEBUG("Router::get_http_response(): return callback corresponding to: ", url_path);
  return callback(request);
}