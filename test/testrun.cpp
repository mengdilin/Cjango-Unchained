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
  if (argc != 2) {
    printf("usage: ./testrun 8081\n");
    return -1;
  }
  std::istringstream iss(argv[1]);
  int port_number;
  iss >> port_number;

  App app;
#ifndef CJANGO_DYNLOAD
  // if CJANGO_DYNLOAD, load "callbacks/url-pattern.json"
  app.add_route("/abc", callback_1);
  app.add_route("/efg/[0-9]{4}/[0-9]{2}", callback_1);
#endif
  //app.add_route("/efg", callback_2);
  app.run(port_number);

  return 0;
}

