#include <cjango>
#include <chrono>
#include <sstream>
#include <iomanip>

std::string datetime_now() {
  auto now = std::chrono::system_clock::now();
  std::time_t now_clock = std::chrono::system_clock::to_time_t(now);
  std::stringstream ss;
  ss << std::put_time(std::localtime(&now_clock), "%c");
  return ss.str();
}

extern "C" http::HttpResponse current_datetime(http::HttpRequest request) {
  std::string now = datetime_now();
  std::string html = "<html></body>It is now " + now + "</html>";
  return http::HttpResponse(html);
}
// current_datetime() mimics the django example of
// https://docs.djangoproject.com/en/1.10/topics/http/views/

extern "C" http::HttpResponse callback_mine2(http::HttpRequest request) {
  _DEBUG("Hi, I'm dynamic callback");
  std::string text = "<html>\r\n<body>\r\n"
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