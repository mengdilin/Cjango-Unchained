#include "../app/app.hpp"
#include <sstream>

http::HttpResponse callback_1 (http::HttpRequest req) {
  _DEBUG("callback1 called");
  string text = "<html>\r\n<body>\r\n"
                "<h1>Hi there!</h1>\r\n"
                "<p>This is just a MOCK response :P</p>\r\n"
                "</body>\r\n</html>";
  http::HttpResponse resp(text);

  return resp;
}

int main(int argc, char* argv[])
{
  if (argc != 3) {
    printf("usage: ./manage runserver <port number>\n");
    return -1;
  }
  std::istringstream iss(std::string(argv[1]) + std::string(" ") + std::string(argv[2]));
  std::string command;
  int port_number;
  iss >> command;
  iss >> port_number;

  // std::shared_ptr<spdlog::logger> parse_logger = spdlog::stdout_color_mt("html");
  // std::shared_ptr<spdlog::logger> route_logger = spdlog::stdout_color_mt("route");
  // loggers[parse_logger->name()] = parse_logger;
  // loggers[route_logger->name()] = route_logger;

  _SPDLOG("html", info, "Welcome to spdlog! {} {}", 1, " 23");
  return 0;

  if (command == "runserver") {
    App app;
#ifndef CJANGO_DYNLOAD
    // if CJANGO_DYNLOAD, load "callbacks/url-pattern.json"
    app.add_route("/abc", callback_1);
    app.add_route("/efg/[0-9]{4}/[0-9]{2}", callback_1);
#endif
    app.run(port_number);
  } else {
    printf("invalid command");
  }

  return 0;
}