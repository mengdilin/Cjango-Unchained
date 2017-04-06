#include <iostream>
#include <memory>
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
                "<p>This is just a DYNAMIC response :P</p>\r\n"
                "</body>\r\n</html>";
  HttpResponse resp(text);
  return resp;
}