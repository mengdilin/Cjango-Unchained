#include "router.hh"

void Router::add_route(std::string url_pattern, functor f) {
  patterns_list.push_back(url_pattern);
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
  return "mock";
}

HttpResponse Router::get_http_response(HttpRequest request) {
  std::string url_path = resolve(request);
  functor callback = pattern_to_callback[url_path];
  return callback(request);
}

int main(){
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
  }
  // FIXME should we do r.add_route() after Router instantiation?
  return 0;
}