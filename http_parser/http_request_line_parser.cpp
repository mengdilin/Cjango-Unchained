#include "http_request_line_parser.hpp"


http::HttpRequestLineParser::HttpRequestLineParser() {}
http::HttpRequestLineParser::HttpRequestLineParser(http::HttpStreamReader reader, http::UrlEncodedFormParser url_encoded_form_parser) {
  this->reader = reader;
  this->url_encoded_form_parser = url_encoded_form_parser;
}

http::HttpRequestLine http::HttpRequestLineParser::parse(std::istream& input_stream) {
  std::string request_line = this->reader.get_next_line(input_stream);
  std::cout << request_line << std::endl;
  //split request_line by white spaces
  std::vector<std::string> result;
  std::istringstream iss(request_line);
  for(std::string s; iss >> s;) {
    std::cout << "vector: " << s << std::endl;
    result.push_back(s);
  }
  if (result.size() != 3) {
    std::cout << request_line << std::endl;
    std::cout << "malformed request line: request has more than 3 items on the first line" << std::endl;
    throw "malformed request line: request has more than 3 items on the first line";
  }
  std::cout << "reached get_http_request_line" << std::endl;
  return get_http_request_line(result);
}

http::HttpRequestLine http::HttpRequestLineParser::get_http_request_line(std::vector<std::string> request_line_fields) {
  std::string uri = request_line_fields[1];

  std::vector<std::string> uri_fields;
  auto q_loc = uri.find("?", 0);
  if (q_loc != std::string::npos) {
    uri_fields.push_back(uri.substr(0, q_loc));
    uri_fields.push_back(uri.substr(q_loc+1, uri.length()));
  } else {
    uri_fields.push_back(uri);
    uri_fields.push_back("");
  }

  std::cout << "uri: " << uri << std::endl;
  std::cout << "uri_fileds[0]" << uri_fields[0] << std::endl;

    for (auto & x : request_line_fields) {
    std::cout << "line field: " << x << std::endl;
  }
  std::istringstream str(uri_fields[1]);
  auto params = this->url_encoded_form_parser.get_parameter(str, uri_fields[1].length());
  std::cout << "request line fields 2: " << request_line_fields[2] << std::endl;
  return http::HttpRequestLine(request_line_fields[0], uri, request_line_fields[2], params);
}



