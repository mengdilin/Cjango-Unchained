#include "http_request_body_parser.hpp"
http::HttpRequestBodyParser::HttpRequestBodyParser(){}

http::HttpRequestBodyParser::HttpRequestBodyParser(std::vector<http::UrlEncodedFormParser> parsers) {
  this->url_encoded_form_parsers = parsers;
}


std::unordered_map<std::string, std::string> http::HttpRequestBodyParser::parse(std::istream& input_stream, std::string content_type, int content_leng) {
  for (auto& parser : url_encoded_form_parsers) {
    if (parser.can_parse_content_type(content_type)) {
      return parser.get_parameter(input_stream, content_leng);
    } else {
      throw "cannot parse content type: " + content_type;
    }
  }
}
