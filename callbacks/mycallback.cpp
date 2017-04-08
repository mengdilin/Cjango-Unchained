#include "../app/externs.hpp"
#include "../routing/router.hpp"

// class MyCallbackView : public View {
// public:
//   MyCallbackView() {
//     _DEBUG("constructor");
//   }
//   ~MyCallbackView() {
//     _DEBUG("de-constructor");
//   }
//   void callback() const override {
//     _DEBUG("callback is called");
//   }
// };

// extern "C" std::shared_ptr<View> get_view() {
//   return std::shared_ptr<View>(new MyCallbackView);
// }

extern "C" HttpResponse callback_mine(HttpRequest request) {
  _DEBUG("Hi, I'm dynamic callback");
  string text = "<html>\r\n<body>\r\n"
                "<h1>Hi there!</h1>\r\n"
                "<h1>This is just a DYNAMIC response version 1 :P</h1>\r\n"
                "</body>\r\n</html>";
  HttpResponse resp(text);
  return resp;
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