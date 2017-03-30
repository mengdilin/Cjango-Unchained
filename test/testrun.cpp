#include "../app/app.hpp"

HttpResponse callback_1 (HttpRequest req) {
  _DEBUG("callback1 called");
  string text = "<html>\r\n<body>\r\n"
                "<h1>Hi there!</h1>\r\n"
                "<p>This is just a MOCK response :P</p>\r\n"
                "</body>\r\n</html>";
  HttpResponse resp(text);

  return resp;
}

int main(int argc, char* argv[])
{
    App app;
    app.add_route("/abc", callback_1);
    //app.add_route("/efg", callback_2);
    app.run(8080);

    return 0;
}

