#include <cjango>

extern "C" http::HttpResponse render_cjango_welcome_page(http::HttpRequest request) {
  return http::HttpResponse::render_to_response("welcome.html");
}
