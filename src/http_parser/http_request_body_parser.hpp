#ifndef LIBHttpRequestBodyParser
#define LIBHttpRequestBodyParser
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include "url_encoded_form_parser.hpp"
namespace http {
  class HttpRequestBodyParser {
  public:
    std::vector<UrlEncodedFormParser> url_encoded_form_parsers; /*!< parser that parses application/x-www-form-urlencoded */

  public:
    HttpRequestBodyParser();
    HttpRequestBodyParser(std::vector<UrlEncodedFormParser>);
    std::unordered_map<std::string, std::string> parse(std::istream&, std::string, int);
  };
}
#endif
