#include <cjango>

extern "C" http::HttpResponse hello_world(http::HttpRequest request) {
  return http::HttpResponse("HelloWorld");
}
