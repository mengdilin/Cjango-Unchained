#include "../app/app.hpp"

HttpResponse callback_1 (HttpRequest req) {
  HttpResponse a = HttpResponse();
  _DEBUG("callback1 called\n\n");
  return a;
}

int main(int argc, char* argv[])
{
    App app;
    app.add_route("/abc", callback_1);
    //app.add_route("/efg", callback_2);
    app.run(8080);

    return 0;
}

