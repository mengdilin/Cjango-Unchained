#include "router.hh"

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
    printf("catched: url pattern is already registered \n");
    return;
  }
  patterns_list.push_back(url_pattern);
  pattern_to_callback[url_pattern] = f;
  printf("add a new rule: %s\n", url_pattern.c_str());
}

HttpResponse callback_1 (HttpRequest req) {
  HttpResponse a = HttpResponse();
  return a;
}

HttpResponse callback_2 (HttpRequest req) {
  HttpResponse b = HttpResponse();
  return b;
}

std::string Router::resolve(HttpRequest request) {
  // return its corresponding url_pattern
  // MAYBE-LATER map request.path -> url_pattern
  std::string url_pattern = request.path;
  if (registered(patterns_list, url_pattern)) {
    printf("resolve(): registered\n");
    return url_pattern;
  } else {
    printf("resolve(): non registered\n");
    return cjango::INVALIDURL;
  }
  // "resolver_match: a resolved url. This attribute is only set after URL resolving took place"
}

HttpResponse Router::get_http_response(HttpRequest request) {
  std::string url_path = resolve(request);
  if (url_path == cjango::INVALIDURL) {
    printf("Router::get_http_response(): this HttpRequest.path is invalid\n");
    return HttpResponse();
  }
  functor callback = pattern_to_callback[url_path];
  printf("Router::get_http_response(): return callback\n");
  return callback(request);
}

int main(){
  HttpRequest req1("/abc");
  HttpRequest req2("/non-registered");
  printf("%s", req1.path.c_str());
  if (false) { // Approach 1:
    // API spec says this way but I think the approach 2 would be much alike django
    URLmap m;
    m["/abc"] = callback_1;
    m["/efg"] = callback_2;
    Router r(m);
  } else { // approach 2
    Router router;
    router.add_route("/abc", callback_1);
    router.add_route("/efg", callback_2);
    router.get_http_response(req1);
    router.get_http_response(req2);
  }
  // FIXME should we do r.add_route() after Router instantiation?
  return 0;
}