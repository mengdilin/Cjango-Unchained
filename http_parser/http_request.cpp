#include "http_request.hpp"

http::HttpRequest::HttpRequest(
  std::string method,
  std::string path,
  std::string scheme,
  std::unordered_map<std::string, std::string> meta,
  std::unordered_map<std::string, std::string> params) {
  this->method = method;
  this->path = path;
  this->scheme = scheme;
  this->meta = meta;
  this->parameters = params;
}
std::ostream& http::operator<<(std::ostream& Str, const http::HttpRequest& v) {
  std::string result = "method: " + v.get_method() + "\n"
            + "path: " + v.get_path()  + "\n"
            + "scheme: " + v.get_scheme() + "\n";
  result += "request headers: \n";
  for (auto& entry : v.get_meta()) {
    result  += entry.first + ":" + entry.second + "\n";
  }
  result += "parameters: \n";

  for (auto& entry : v.get_parameters()) {
    result  += entry.first + ":" + entry.second + "\n";
  }

  return Str << result;
}

std::string http::HttpRequest::get_method() const {
  return this->method;
}
std::string http::HttpRequest::get_path() const {
  return this->path;
}
std::string http::HttpRequest::get_scheme() const {
  return this->scheme;
}
std::unordered_map<std::string, std::string> const & http::HttpRequest::get_meta() const {
  return this->meta;
}
std::unordered_map<std::string, std::string> const & http::HttpRequest::get_parameters() const {
  return this->parameters;
}
