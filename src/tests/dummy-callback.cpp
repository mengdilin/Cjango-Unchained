#include <cjango>

extern "C" http::HttpResponse dummy(http::HttpRequest request) {
  return http::HttpResponse("dummy");
}