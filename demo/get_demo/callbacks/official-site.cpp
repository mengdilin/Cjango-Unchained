#include <cjango>

extern "C" http::HttpResponse render_official_site(http::HttpRequest request) {
  return http::HttpResponse::render_to_response("../templates/official.html", request);
}

extern "C" http::HttpResponse get_logo(http::HttpRequest request) {
  return http::HttpResponse::render_to_response("../static/logo.png", "image/png", request);
}