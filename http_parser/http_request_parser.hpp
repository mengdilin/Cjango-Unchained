#ifndef MyHttpRequestParserHeader
#define MyHttpRequestParserHeader
#include "url_encoded_form_parser.hpp"
#include "http_request_line_parser.hpp"
#include "http_request_headers_parser.hpp"
#include "http_request_body_parser.hpp"
#include "http_request.hpp"
#include <unordered_map>
#include <iostream>
#include <fstream>

namespace http {
  class HttpRequestParser {
  public:
    HttpRequestLineParser line_parser;
    HttpRequestHeadersParser headers_parser;
    HttpRequestBodyParser body_parser;
  public:
    HttpRequestParser();
    HttpRequestParser(HttpRequestLineParser, HttpRequestHeadersParser,HttpRequestBodyParser);
    HttpRequest parse_request_line_and_headers(std::istream& input_stream);
    std::unordered_map<std::string, std::string> parse_body(std::istream&, std::string, int);

  };
}
#endif
