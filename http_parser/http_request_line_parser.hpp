#ifndef LIBHttpRequestLineParser
#define LIBHttpRequestLineParser
#include "http_request_line.hpp"
#include "http_stream_reader.hpp"
#include "url_encoded_form_parser.hpp"
#include <string>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
namespace http {
class HttpRequestLineParser {
  public:
    HttpStreamReader reader;
    UrlEncodedFormParser url_encoded_form_parser;

  public:
    HttpRequestLineParser();
    HttpRequestLineParser(HttpStreamReader reader, UrlEncodedFormParser url_encoded_form_parser);
    HttpRequestLine parse(std::istream& input_stream);
    HttpRequestLine get_http_request_line(std::vector<std::string> request_line_fields);

};
}
#endif
