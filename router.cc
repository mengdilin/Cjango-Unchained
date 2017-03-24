#include "router.hh"

void Router::add_route(std::string url_pattern, functor f) {
  patterns_list.push_back(url_pattern);
  printf("add a new rule: %s\n", url_pattern.c_str());
}

HttpResponse callback_1 (HttpRequest) {
  HttpResponse a = HttpResponse();
  return a;
}

int main(){
  URLmap m;
  m["/abc"] = callback_1;
  Router r(m);
  return 0;
}