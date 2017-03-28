#ifndef LIBHttpRequestHeadersParser
#define LIBHttpRequestHeadersParser
#include "http_stream_reader.hpp"
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <vector>
namespace http {
  class HttpRequestHeadersParser {
  public:
    HttpStreamReader reader;
  public:
    HttpRequestHeadersParser();
    HttpRequestHeadersParser(HttpStreamReader reader);
    std::unordered_map<std::string, std::string> parse(std::istream&);
  };
}
#endif
