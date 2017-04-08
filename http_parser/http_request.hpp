#ifndef MyHttpRequestHeader
#define MyHttpRequestHeader
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <string>
namespace http {
  class HttpRequest {
  private:
    std::string method;
    std::string path;
    std::string scheme;
    std::unordered_map<std::string, std::string> meta;
    std::unordered_map<std::string, std::string> parameters;
    std::unordered_map<std::string, std::string> cookie;
  public:
    HttpRequest(std::string path) : path(path) {}; // for easy testing
    HttpRequest(std::string,
      std::string, std::string,
      std::unordered_map<std::string, std::string>,
      std::unordered_map<std::string, std::string>,
      std::unordered_map<std::string, std::string>);
  public:
    std::string get_method() const;
    std::string get_path() const;
    std::string get_scheme() const;
    std::unordered_map<std::string, std::string> const & get_meta() const;
    std::unordered_map<std::string, std::string> const & get_parameters() const;
    std::unordered_map<std::string, std::string> const & get_cookie() const;

  };
  std::ostream& operator<<(std::ostream& Str, HttpRequest const & v);
}

#endif
