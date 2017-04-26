#include "../app/app.hpp"
#include <sstream>
#include "../lib/json.hpp"
#include "../app/externs.hpp"

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
  if (argc < 3) {
    printf("usage: ./manage runserver <port number>\n");
    return -1;
  }
  std::istringstream iss(std::string(argv[1]) + std::string(" ") + std::string(argv[2]));
  std::string command;
  int port_number;
  iss >> command;
  iss >> port_number;

#ifdef DEBUG
  if (argc > 3 && std::string(argv[3]) == "--whitelist") {
    for (int i = 4; i < argc; ++i)
      cjango_loggers.whitelist.insert(argv[i]);
    for (int i = 4; i < argc; ++i) // should be after inserting all
      _SPDLOG("init", info, "whitelist is specified: {}", argv[i]);
  }
#endif



  // std::shared_ptr<spdlog::logger> parse_logger = spdlog::stdout_color_mt("html");
  // std::shared_ptr<spdlog::logger> route_logger = spdlog::stdout_color_mt("route");
  // loggers[parse_logger->name()] = parse_logger;
  // loggers[route_logger->name()] = route_logger;

  _SPDLOG("html", info, "Welcome to spdlog! {} {}", 1, " 23");

  std::string static_root_dir;
  // set mappings of URL -> static files
  std::ifstream i("../json/settings.json");
  nlohmann::json j;
  i >> j;
  for (nlohmann::json::iterator it = j.begin(); it != j.end(); ++it) {
    if (it.key() == std::string("STATIC_URL"))
      static_root_dir = it.value();

    if (it.key() == std::string("TEMPLATES"))
      g_templates_root_dir = it.value(); // FIXME global variable

    if (it.key() == std::string("CALLBACKS"))
      g_callbacks_root_dir = it.value();
  }

  if (command == "runserver") {
    App app;
#ifndef CJANGO_DYNLOAD
    // if CJANGO_DYNLOAD, load "json/urls.json"
    app.add_route("/abc", callback_1);
    app.add_route("/efg/[0-9]{4}/[0-9]{2}", callback_1);
#endif
    app.router.set_static_root_dir(static_root_dir);
    app.run(port_number);
  } else {
    printf("invalid command");
  }

  return 0;
}