#include <cjango>
#include <sstream>
#include <iomanip>
#include <string>
#include <fstream>
#include <streambuf>
inline std::string datetime_now() {
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
