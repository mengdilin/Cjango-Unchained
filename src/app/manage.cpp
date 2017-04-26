#include "../app/app.hpp"
#include <sstream>
#include <unistd.h>
#include "../lib/json.hpp"
#include "../app/externs.hpp"
#include "../http_parser/http_response.hpp"
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
    printf("usage: ./manage runserver <port number>  [--setting <settings.json path>]\n");
    return -1;
  }
  std::istringstream iss(std::string(argv[1]) + std::string(" ") + std::string(argv[2]));
  std::string command;
  int port_number;
  iss >> command;
  iss >> port_number;
  std::string settings_path = "../json/settings.json"; // default
  if (argc == 5) {
    if (strncmp(argv[3], "--setting", strlen("--setting")) == 0) {
      settings_path = argv[4];
    }
  }

// FIXME --whitelist and --settings
#ifdef DEBUG
  if (argc > 3 && std::string(argv[3]) == "--whitelist") {
    for (int i = 4; i < argc; ++i)
      cjango_loggers.whitelist.insert(argv[i]);
    for (int i = 4; i < argc; ++i) // should be after inserting all
      _SPDLOG("init", info, "whitelist is specified: {}", argv[i]);
  }
#endif

  App app;

  // set mappings of URL -> static files
  std::ifstream i(settings_path);
  _SPDLOG("path", info, "settings: {}", settings_path);
  nlohmann::json j;
  try {
    // The central problem is that C++ doesn't have default chdir() command
    // and it's only in Boost::FileSystem -- and other cross-platform libraries are also hard to install.
    // Here, just specify parent dir of src/ as root
    // This is a dirty hack and prohibit moving ./manage executable
    // to the parent dir (no libhttpresponse.so error)
    std::string parent_dir = "../";
    i >> j;
    for (nlohmann::json::iterator it = j.begin(); it != j.end(); ++it) {
      std::string val = it.value();
      if (it.key() == std::string("STATIC_URL"))
        app.router.set_static_dir(parent_dir + val);
      if (it.key() == std::string("TEMPLATES"))
        http::HttpResponse::templates_root = parent_dir + val;
      if (it.key() == std::string("CALLBACKS"))
        g_callbacks_root_dir = parent_dir + val; // FIXME local
      if (it.key() == std::string("URLS_JSON"))
        app.set_urls_json_dir(parent_dir + val); // FIXME local
    }
  } catch (const std::exception& e) {
    _SPDLOG("path", error, "json error: {}", e.what());
    return 1;
  }
  _SPDLOG("path", info, "CALLBACKS:  {}", g_callbacks_root_dir);
  _SPDLOG("path", info, "URLS_JSON:  {}", app.get_urls_json_dir());
  _SPDLOG("path", info, "STATIC_URL: {}", app.router.get_static_dir());
  _SPDLOG("path", info, "TEMPLATES:  {}", http::HttpResponse::templates_root);


  if (command == "runserver") {

#ifdef CJANGO_DYNLOAD
    app.add_monitored_dir(app.get_urls_json_dir());
    app.reload_url_mappings();
#else
    app.add_route("/abc", callback_1);
    app.add_route("/efg/[0-9]{4}/[0-9]{2}", callback_1);
#endif
    app.run(port_number);

  } else {

    printf("invalid command");

  }

  return 0;
}
