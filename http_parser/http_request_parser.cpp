#include "http_request_parser.hpp"

http::HttpRequestParser::HttpRequestParser() {}


http::HttpRequestParser::HttpRequestParser(
  http::HttpRequestLineParser line_parser,
  http::HttpRequestHeadersParser headers_parser,
  http::HttpRequestBodyParser body_parser) {
  this->line_parser = line_parser;
  this->headers_parser = headers_parser;
  this->body_parser = body_parser;
}

http::HttpRequest http::HttpRequestParser::parse_request_line_and_headers(std::istream& input_stream) {
  HttpRequestLine request_line = line_parser.parse(input_stream);
  std::unordered_map<std::string, std::string> request_headers = headers_parser.parse(input_stream);
  return HttpRequest(request_line.action, request_line.uri, request_line.protocolVersion, request_headers, request_line.parameters);
}
std::unordered_map<std::string, std::string> http::HttpRequestParser::parse_body(std::istream& input_stream, std::string content_type, int content_leng) {
  return body_parser.parse(input_stream, content_type, content_leng);
}
