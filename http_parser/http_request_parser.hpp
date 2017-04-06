#ifndef MyHttpRequestParserHeader
#define MyHttpRequestParserHeader
#include "url_encoded_form_parser.hpp"
#include "http_request_body_parser.hpp"
#include "http_request.hpp"
#include "http_request_line.hpp"
#include <unordered_map>
#include <iostream>
#include <fstream>

namespace http {
  class HttpRequestParser {
  public:
    HttpRequestBodyParser body_parser;
    HttpStreamReader reader;
    UrlEncodedFormParser url_encoded_form_parser;
  public:
    HttpRequestParser();
    HttpRequestParser(HttpRequestBodyParser);
    HttpRequest parse_request_line_and_headers(std::istream& input_stream);
    std::unordered_map<std::string, std::string> parse_body(std::istream&, std::string, int);
  private:
    HttpRequestLine get_http_request_line(std::vector<std::string> request_line_fields) ;
    std::unordered_map<std::string, std::string> parse_head(std::istream& input_stream);
    HttpRequestLine parse_line(std::istream& input_stream);
    std::unordered_map<std::string, std::string> get_http_cookie(std::unordered_map<std::string, std::string>&);

  };
}
#endif
