#ifndef LIBUrlEncodedFormParser
#define LIBUrlEncodedFormParser
#include "http_stream_reader.hpp"
#include <string>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <exception>
#include <vector>
#include <sstream>
namespace http {

class UrlEncodedFormParser {
  public:
    HttpStreamReader input_stream_reader;
    std::string supported_content_type = "application/x-www-form-urlencoded";


  public:
    UrlEncodedFormParser();
    bool can_parse_content_type(std::string content_type);
    char charToInt(char);
    char strToBin(char *pString);
    std::string urlDecode(const std::string &str);
    /*
     * using uri decode functionality found via codeguru.com
     * source: http://www.codeguru.com/cpp/cpp/algorithms/strings/article.php/c12759/URI-Encoding-and-Decoding.htm
     */
    std::string uri_decode(const std::string & sSrc);

    std::unordered_map<std::string, std::string> get_parameter(std::istream& input_stream, int content_leng);

    std::vector<std::string> split(std::string str, char delimiter);

};
}
#endif
