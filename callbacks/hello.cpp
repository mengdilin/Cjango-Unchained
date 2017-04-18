#include <cjango>

extern "C" http::HttpResponse callback_hello_world(http::HttpRequest request) {
  return http::HttpResponse("helloWorld");
}
