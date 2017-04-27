#include "http_request_body_parser.hpp"
#include "../app/externs.hpp"

/** @file http_request_body_parser.cpp
 * \ingroup http
 * @brief HttpRequestBodyParser class implementation
 */

/**
* @brief HttpRequestBodyParser constructor that contains a default UrlEncodedFormParser for form parsing
* @return HttpRequestBodyParser
*/
http::HttpRequestBodyParser::HttpRequestBodyParser(){
  http::UrlEncodedFormParser parser;
  this->url_encoded_form_parsers.push_back(parser);
}

http::HttpRequestBodyParser::HttpRequestBodyParser(std::vector<http::UrlEncodedFormParser> parsers) {
  this->url_encoded_form_parsers = parsers;
}

/**
* @brief given a http request body in istream containing http request parameters,
* it will parse the content of istream and loads the http request parameters to
* an unordered_map
* @return an unordered_map containing the parameters for HttpRequest
*/
std::unordered_map<std::string, std::string> http::HttpRequestBodyParser::parse(std::istream& input_stream, std::string content_type, int content_leng) {
  for (auto& parser : url_encoded_form_parsers) {
    if (parser.can_parse_content_type(content_type)) {
      return parser.get_parameter(input_stream, content_leng);
    } else {
      _DEBUG("cannot parse content type: ", content_type);
    }
  }
  throw "cannot parse content type: " + content_type;
}
