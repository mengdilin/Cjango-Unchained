#include "../app/externs.hpp"
#include "../routing/router.hpp"
#include <chrono>
#include <sstream>
#include <iomanip>

auto datetime_now() {
  auto now = std::chrono::system_clock::now();
  std::time_t now_clock = std::chrono::system_clock::to_time_t(now);
  return std::put_time(std::localtime(&now_clock), "%c");
}

// mimic the example of https://docs.djangoproject.com/en/1.10/topics/http/views/
extern "C" HttpResponse current_datetime(HttpRequest request) {
  auto now = datetime_now();
  std::stringstream html;
  html << "<html></body>It is now " << now << "</html>";
  return HttpResponse(html.str());
}

extern "C" HttpResponse callback_mine2(HttpRequest request) {
  _DEBUG("Hi, I'm dynamic callback");
  string text = "<html>\r\n<body>\r\n"
                "<h1>Hi there!</h1>\r\n"
                "<h1>This is just a DYNAMIC response version 2 :D</h1>\r\n"
                "<h1>Hi there!</h1>\r\n"
                "</body>\r\n</html>";
  HttpResponse resp(text);
  return resp;
}

extern "C" HttpResponse callback_mine3(HttpRequest request) {
  _DEBUG("Hi, I'm dynamic callback");
  string text = "<html>\r\n<body>\r\n"
                "<h1>Hi there!</h1>\r\n"
                "<h1>Hi there!</h1>\r\n"
                "<h1>This is just a Columbia response version 3 :) </h1>\r\n"
                "<h1>Hi there!</h1>\r\n"
                "<h1>Hi there!</h1>\r\n"
                "</body>\r\n</html>";
  HttpResponse resp(text);
  return resp;
}