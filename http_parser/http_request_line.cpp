#include "http_request_line.hpp"

http::HttpRequestLine::HttpRequestLine(std::string action, std::string uri, std::string protocolVersion, std::unordered_map<std::string, std::string>& parameters) {
      this->action = action;
      this->uri = uri;
      this->parameters = parameters;
      this->protocolVersion = protocolVersion;
}
