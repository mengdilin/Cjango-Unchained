#include "http_request_parser.hpp"

http::HttpRequestParser::HttpRequestParser() {}


http::HttpRequestParser::HttpRequestParser(
  http::HttpRequestBodyParser body_parser) {
  this->body_parser = body_parser;
}

http::HttpRequest http::HttpRequestParser::parse_request_line_and_headers(std::istream& input_stream) {
  HttpRequestLine request_line = parse_line(input_stream); //line_parser
  std::unordered_map<std::string, std::string> request_headers = parse_head(input_stream); //header_parser
  return HttpRequest(request_line.action, request_line.uri, request_line.protocolVersion, request_headers, request_line.parameters);
}
std::unordered_map<std::string, std::string> http::HttpRequestParser::parse_body(std::istream& input_stream, std::string content_type, int content_leng) {
  return body_parser.parse(input_stream, content_type, content_leng);
}

http::HttpRequestLine http::HttpRequestParser::parse_line(std::istream& input_stream) {
  std::string request_line = this->reader.get_next_line(input_stream);
  std::cout << request_line << std::endl;
  //split request_line by white spaces
  std::vector<std::string> result;
  std::istringstream iss(request_line);
  for(std::string s; iss >> s;) {
    //std::cout << "vector: " << s << std::endl;
    result.push_back(s);
  }
  if (result.size() != 3) {
    // std::cout << request_line << std::endl;
    std::cout << "malformed request line: request has != 3 items on the first line" << std::endl;
    throw "malformed request line: request has != 3 items on the first line";
    // if you access localhost:8080 by browser and leave it for a few seconds,
    // A request(s?) comes with 0 item on the first line
  }
  //std::cout << "reached get_http_request_line" << std::endl;
  return get_http_request_line(result);
}

http::HttpRequestLine http::HttpRequestParser::get_http_request_line(std::vector<std::string> request_line_fields) {
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

  //std::cout << "uri: " << uri << std::endl;
  //std::cout << "uri_fileds[0]" << uri_fields[0] << std::endl;
  /*
    for (auto & x : request_line_fields) {
    std::cout << "line field: " << x << std::endl;
  }
  */
  std::istringstream str(uri_fields[1]);
  auto params = this->url_encoded_form_parser.get_parameter(str, uri_fields[1].length());
  //std::cout << "request line fields 2: " << request_line_fields[2] << std::endl;
  return http::HttpRequestLine(request_line_fields[0], uri, request_line_fields[2], params);
}

std::unordered_map<std::string, std::string> http::HttpRequestParser::parse_head(std::istream& input_stream) {
  std::unordered_map<std::string, std::string> request_headers;
  std::string next;
  while((next = this->reader.get_next_line(input_stream)) != "") {
    auto colon_loc = next.find(":", 0);
    if (colon_loc == std::string::npos || colon_loc == 0 || colon_loc == next.length()) {
      std::cout << "malformed header format " << next << std::endl;
      throw "malformed header format " + next;
    } else {
      request_headers.insert({next.substr(0, colon_loc), next.substr(colon_loc+1, next.length())});
    }

  }
  return request_headers;
}
