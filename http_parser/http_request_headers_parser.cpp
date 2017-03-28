#include "http_request_headers_parser.hpp"

http::HttpRequestHeadersParser::HttpRequestHeadersParser() {}
http::HttpRequestHeadersParser::HttpRequestHeadersParser(http::HttpStreamReader reader) {
  this->reader = reader;
}
std::unordered_map<std::string, std::string> http::HttpRequestHeadersParser::parse(std::istream& input_stream) {
  std::unordered_map<std::string, std::string> request_headers;
  std::string next;
  while((next = this->reader.get_next_line(input_stream)) != "") {
    auto colon_loc = next.find(":", 0);
    if (colon_loc == std::string::npos || colon_loc == 0 || colon_loc == next.length()) {
      throw "malformed header format " + next;
    } else {
      request_headers.insert({next.substr(0, colon_loc), next.substr(colon_loc+1, next.length())});
    }

  }
  return request_headers;
}
