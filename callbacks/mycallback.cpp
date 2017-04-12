#include <cjango>

extern "C" http::HttpResponse callback_mine(http::HttpRequest request) {
  _DEBUG("Hi, I'm dynamic callback");
  string text = "<html>\r\n<body>\r\n"
                "<h1>Hi there!</h1>\r\n"
                "<h1>This is just a DYNAMIC response version 1 :P</h1>\r\n"
                "</body>\r\n</html>";
  http::HttpResponse resp(text);
  return resp;
}

extern "C" http::HttpResponse callback_mine2(http::HttpRequest request) {
  _DEBUG("Hi, I'm dynamic callback");
  string text = "<html>\r\n<body>\r\n"
                "<h1>Hi there!</h1>\r\n"
                "<h1>This is just a DYNAMIC response version 2 :D</h1>\r\n"
                "<h1>Hi there!</h1>\r\n"
                "</body>\r\n</html>";
  http::HttpResponse resp(text);
  return resp;
}

extern "C" http::HttpResponse callback_mine3(http::HttpRequest request) {
  _DEBUG("Hi, I'm dynamic callback");
  string text = "<html>\r\n<body>\r\n"
                "<h1>Hi there!</h1>\r\n"
                "<h1>Hi there!</h1>\r\n"
                "<h1>This is just a Columbia response version 3 :) </h1>\r\n"
                "<h1>Hi there!</h1>\r\n"
                "<h1>Hi there!</h1>\r\n"
                "</body>\r\n</html>";
  http::HttpResponse resp(text);
  return resp;
}

extern "C" http::HttpResponse page_home(http::HttpRequest request) {
  //return http::HttpResponse("blah");
  auto resp = http::HttpResponse::render_to_response("./templates/home.html");
  _DEBUG(resp.to_string());
  return http::HttpResponse::render_to_response("./templates/home.html");
}
// current_datetime() mimics the django example of
// https://docs.djangoproject.com/en/1.10/topics/http/views/

extern "C" http::HttpResponse page_index(http::HttpRequest request) {

  auto resp = http::HttpResponse::render_to_response("./templates/index.html");
  _DEBUG(resp.to_string());
  return resp;
}
