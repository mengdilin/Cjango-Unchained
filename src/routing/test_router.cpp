#include "router.hpp"


HttpResponse callback_1 (HttpRequest req) {
  HttpResponse a = HttpResponse();
  return a;
}

HttpResponse callback_2 (HttpRequest req) {
  HttpResponse b = HttpResponse();
  return b;
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