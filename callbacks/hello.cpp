#include <cjango>

extern "C" auto hello_world(http::HttpRequest request) {
  return http::HttpResponse("helloWorld");
}
