#ifndef MyHttpResponseHeader
#define MyHttpResponseHeader
#include <unordered_map>
#include <string>

namespace http {
  class HttpResponse {
  public:
    std::string content;
    int status_code = 200;//default
    std::string reason_phrase = "OK";//default
    std::string http_version = "HTTP/1.0";//default
    static std::unordered_map<int, std::string> code_to_reason;
    std::string content_type = "text/html"; //default
    std::unordered_map<std::string, std::string> headers;
  private:
    void set_cookie(std::string, std::string);
  public:
    //default good http response
    HttpResponse(std::string content) {
      this->content = content;
      headers.insert({"Content-Type", content_type});
      //_DEBUG("content_length: ", content_type.length());
      headers.insert({"Content-Length", std::to_string(content.length())});
      //headers.insert({"Content-Length", std::to_string(content_type.length())});
    }
    HttpResponse(std::string, std::string);
    HttpResponse(int); //meant for bad requests
    std::string to_string();

  };
  std::ostream& operator<<(std::ostream& Str, HttpResponse const & v);

}
#endif
