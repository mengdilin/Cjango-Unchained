#ifndef MyHttpResponseHeader
#define MyHttpResponseHeader
#include <unordered_map>
#include <string>
#include "http_request.hpp"

namespace http {
  class HttpResponse {

  public:
    std::string content;
    int status_code = 200;//default
    std::string reason_phrase = "OK";//default
    std::string http_version = "HTTP/1.0";//default
    static std::unordered_map<int, std::string> code_to_reason;
    static std::string templates_root;
    std::string content_type = "text/html"; //default
    std::unordered_map<std::string, std::string> headers;

  public:
    //default good http response
    HttpResponse(std::string);
    HttpResponse(std::string, HttpRequest&);
    HttpResponse(std::string, std::string);
    HttpResponse(int); //meant for bad requests
    std::string to_string();
    void set_cookie(std::string, std::string);
    static HttpResponse render_to_response(std::string);
    static HttpResponse render_to_response(std::string, std::string);
    static HttpResponse render_to_response(std::string, HttpRequest&);
    static HttpResponse render_to_response(std::string, std::string, HttpRequest&);
    static std::string get_template(std::string path);

  };

  std::ostream& operator<<(std::ostream& Str, HttpResponse const & v);


}
#endif
