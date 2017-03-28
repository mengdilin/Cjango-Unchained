#include "http_request.hpp"

http::HttpRequest::HttpRequest(
  std::string action,
  std::string url,
  std::string version,
  std::unordered_map<std::string, std::string> request_headers,
  std::unordered_map<std::string, std::string> params) {
  this->action = action;
  this->url = url;
  this->version = version;
  this->request_headers = request_headers;
  this->parameters = params;
}
std::ostream& http::operator<<(std::ostream& Str, const http::HttpRequest& v) {
  std::string result = "action: " + v.action + "\n"
            + "url: " + v.url  + "\n"
            + "version: " + v.version + "\n";
  result += "request headers: \n";
  for (auto& entry : v.request_headers) {
    result  += entry.first + ":" + entry.second + "\n";
  }
  for (auto& entry : v.parameters) {
    result  += entry.first + ":" + entry.second + "\n";
  }

  return Str << result;
}
