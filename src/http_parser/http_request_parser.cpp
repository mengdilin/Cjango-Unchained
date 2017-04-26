#include "http_request_parser.hpp"
#include "../app/externs.hpp"
#include <algorithm>
#include <string>
#include <iostream>
http::HttpRequestParser::HttpRequestParser() {}


http::HttpRequestParser::HttpRequestParser(
  http::HttpRequestBodyParser body_parser) {
  this->body_parser = body_parser;
}

/**
** @brief given an input_stream containing a http request, parses the request line and headers
** @return a http request with populated headers and an empty body
*/
http::HttpRequest http::HttpRequestParser::parse_request_line_and_headers(std::istream& input_stream) {
  HttpRequestLine request_line = parse_line(input_stream); //line_parser
  std::unordered_map<std::string, std::string> request_headers = parse_head(input_stream); //header_parser

  return HttpRequest(request_line.action, request_line.uri, request_line.protocolVersion, request_headers, request_line.parameters, get_http_cookie(request_headers));
}

/**
** @brief given an input_stream containing a http request, parses the request line, headers, and body
** @return a http request object representing the data from the input_stream
*/
http::HttpRequest http::HttpRequestParser::parse(std::istream& input_stream) {

  HttpRequestLine request_line = parse_line(input_stream); //line_parser
  std::unordered_map<std::string, std::string> request_headers = parse_head(input_stream); //header_parser
  if (request_line.action == "GET") {
    return HttpRequest(request_line.action, request_line.uri, request_line.protocolVersion, request_headers, request_line.parameters, get_http_cookie(request_headers));
  } else if (request_line.action=="POST") {
    auto content_type_result = request_headers.find("Content-Type");
    //if we cannot find content-type specified or content-type does not contain
    //the form type we support, we skip body parsing
    if ((content_type_result == request_headers.end()) ||
      (content_type_result->second).find("application/x-www-form-urlencoded") == std::string::npos) {
      std::unordered_map<std::string, std::string> body_params;
      return HttpRequest(
      request_line.action,
      request_line.uri,
      request_line.protocolVersion,
      request_headers,
      body_params,
       get_http_cookie(request_headers)
      );
    }
    std::stringstream ss;
    ss << input_stream.rdbuf();
    return HttpRequest(
      request_line.action,
      request_line.uri,
      request_line.protocolVersion,
      request_headers,
      parse_body(ss, "application/x-www-form-urlencoded", ss.str().length()),
      get_http_cookie(request_headers)
      );

  } else {
    return HttpRequest(request_line.action, request_line.uri, request_line.protocolVersion, request_headers, request_line.parameters, get_http_cookie(request_headers));
  }


}

/**
** @brief helper method that given a string of encoded cookie key value pairs from parameters map, creates a cookie map,
*/
std::unordered_map<std::string, std::string> http::HttpRequestParser::get_http_cookie(std::unordered_map<std::string, std::string>& params) {
  std::unordered_map<std::string, std::string> cookies;

  //get cookie value from params and parse
  auto result = params.find("Cookie");
  if (result != params.end()) {
    auto value = result->second;
      std::vector<std::string> cookie_pairs = url_encoded_form_parser.split(value, ';');
      for (auto pair : cookie_pairs) {
        _SPDLOG(cjango_loggers.http, info, "cookie pair: {}",  pair);
        pair.erase(std::remove_if(pair.begin(), pair.end(), isspace),
              pair.end());
         auto q_loc = pair.find("=", 0);
         if (q_loc != std::string::npos) {


          cookies.insert({pair.substr(0, q_loc), pair.substr(q_loc+1, pair.length())});
       }
      }
  }
  return cookies;

}

std::unordered_map<std::string, std::string> http::HttpRequestParser::parse_body(std::istream& input_stream, std::string content_type, int content_leng) {
  return body_parser.parse(input_stream, content_type, content_leng);
}

/**
** @brief helper method that parses the first line of a http request
*/
http::HttpRequestLine http::HttpRequestParser::parse_line(std::istream& input_stream) {
  std::string request_line = this->reader.get_next_line(input_stream);
  //split request_line by white spaces
  std::vector<std::string> result;
  std::istringstream iss(request_line);
  for(std::string s; iss >> s;) {
    result.push_back(s);
  }
  if (result.size() != 3) {
    _SPDLOG(cjango_loggers.http, warn, "malformed request line: request has != 3 items on the first line");
    throw "malformed request line: request has != 3 items on the first line";
  }
  return get_http_request_line(result);
}

/**
** @brief helper method that parses get paramters in the first line of a http request
*/
http::HttpRequestLine http::HttpRequestParser::get_http_request_line(std::vector<std::string> request_line_fields) {
  std::string uri = request_line_fields[1];

  std::vector<std::string> uri_fields;
  auto q_loc = uri.find("?", 0);
  auto true_uri = uri.substr(0, q_loc); //true uri path without parameters
  if (q_loc != std::string::npos) {
    uri_fields.push_back(uri.substr(0, q_loc));
    uri_fields.push_back(uri.substr(q_loc+1, uri.length()));
  } else {
    uri_fields.push_back(uri);
    uri_fields.push_back("");
  }

  std::istringstream str(uri_fields[1]);
  _SPDLOG(cjango_loggers.http, info, "uri fields: {}", uri_fields[1]);
  if (request_line_fields[0] == "GET") {
    auto params = this->url_encoded_form_parser.get_parameter(str, uri_fields[1].length());
      return http::HttpRequestLine(request_line_fields[0], true_uri, request_line_fields[2], params);


  } else {
  return http::HttpRequestLine(request_line_fields[0], uri, request_line_fields[2]);

  }

}
/**
** @brief helper method that parses a http request's headers
*/
std::unordered_map<std::string, std::string> http::HttpRequestParser::parse_head(std::istream& input_stream) {
  std::unordered_map<std::string, std::string> request_headers;
  std::string next;
  while((next = this->reader.get_next_line(input_stream)) != "") {
    auto colon_loc = next.find(":", 0);
    if (colon_loc == std::string::npos || colon_loc == 0 || colon_loc == next.length()) {
      _SPDLOG(cjango_loggers.http, info, "malformed header format {}", next);
      throw "malformed header format " + next;
    } else {
      request_headers.insert({next.substr(0, colon_loc), next.substr(colon_loc+1, next.length())});
    }

  }
  return request_headers;
}
