#include <cjango>
#include <chrono>
#include <sstream>
#include <iomanip>

extern "C" http::HttpResponse page_home(http::HttpRequest request) {

  return http::HttpResponse::render_to_response("./templates/home.html");
}
// current_datetime() mimics the django example of
// https://docs.djangoproject.com/en/1.10/topics/http/views/

extern "C" http::HttpResponse page_index(http::HttpRequest request) {

  auto resp = http::HttpResponse::render_to_response("./templates/index.html");
  _DEBUG(resp.to_string());
  return resp;
}
