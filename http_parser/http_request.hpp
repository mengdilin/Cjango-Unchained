#ifndef MyHttpRequestHeader
#define MyHttpRequestHeader
#include <unordered_map>
#include <iostream>
#include <fstream>
namespace http {
  class HttpRequest {
  public:
    std::string action;
    std::string url;
    std::string version;
    std::unordered_map<std::string, std::string> request_headers;
    std::unordered_map<std::string, std::string> parameters;
  public:
    HttpRequest(std::string, std::string, std::string, std::unordered_map<std::string, std::string>,std::unordered_map<std::string, std::string>);

  };
  std::ostream& operator<<(std::ostream& Str, HttpRequest const & v);
}

#endif
