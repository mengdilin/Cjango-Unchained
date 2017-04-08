#include "http_response.hpp"
#include "../app/externs.hpp"

/*
std::unordered_map<std::string, std::string> http::HttpResponse::code_to_reason_init() {
      code_to_reason.insert({200, "OK"});
      code_to_reason.insert({201, "Created"});
      code_to_reason.insert({202, "Accepted"});
      code_to_reason.insert({204, "No Content"});
      code_to_reason.insert({301, "Moved Permanently"});
      code_to_reason.insert({302, "Moved Temporarily"});
      code_to_reason.insert({304, "Not Modified"});
      code_to_reason.insert({400, "Bad Request"});
      code_to_reason.insert({401, "Unauthorized"});
      code_to_reason.insert({403, "Forbidden"});
      code_to_reason.insert({404, "Not Found"});
      code_to_reason.insert({500, "Internal Server Error"});
      code_to_reason.insert({501, "Not Implemented"});
      code_to_reason.insert({502, "Bad Gateway"});
      code_to_reason.insert({503, "Service Unavailable"});
}
*/

// caprice-j moved HttpResponse constructor in http_response.hpp
// in order to instantiate in callback files easily

void http::HttpResponse::set_cookie(std::string key, std::string value) {
  auto result = headers.find("Set-Cookie");
  if (result != headers.end()) {
    result->second += "; "+key+"="+value;
    headers.insert({"Set-Cookie", result->second});

  } else {
    headers.insert({"Set-Cookie", key+"="+value});
  }

}

//default good http response
http::HttpResponse::HttpResponse(std::string content, std::string content_type) {
  this->content = content;
  this->content_type = content_type;
  headers.insert({"Content-Type", content_type});
  //_DEBUG("content_length: ", content_type.length());
  headers.insert({"Content-Length", std::to_string(content.length())});
}

std::string get_reason_phrase(int status_code) {
  switch(status_code) {
    case 200:
      return "OK";
    case 201:
      return "Created";
    case 202:
      return "Accepted";
    case 204:
      return "No Content";
    case 301:
      return "Moved Permanently";
    case 302:
      return "Moved Temporarily";
    case 304:
      return "Not Modified";
    case 400:
      return "Bad Request";
    case 401:
      return "Unauthorized";
    case 403:
      return "Forbidden";
    case 404:
      return "Not Found";
    case 500:
      return "Internal Server Error";
    case 501:
      return "Not Implemented";
    case 502:
      return "Bad Gateway";
    case 503:
      return "Service Unavailable";
    default:
      throw "unrecognized status code: " + std::to_string(status_code);
  }
}

http::HttpResponse::HttpResponse(int status_code) {
  this->status_code = status_code;
  get_reason_phrase(status_code);
  headers.insert({"Content-Type", content_type});
  headers.insert({"Content-Length", std::to_string(content.length())});
}

std::string http::HttpResponse::to_string() {
  std::string result;
  std::string crfl = "\r\n";
  std::string sp = " ";
  result += http_version + sp + std::to_string(status_code) + sp + reason_phrase + crfl;
  for (auto& header_pair : headers) {
    result += header_pair.first + ": " + header_pair.second + crfl;
  }
  result += crfl;
  result += content;
  return result;
}
std::ostream& http::operator<<(std::ostream& Str, const http::HttpResponse& v) {
std::string result;
  std::string crfl = "\r\n";
  std::string sp = " ";

  Str << v.http_version;

  Str << sp;

  Str << std::to_string(v.status_code) ;

  Str << sp;

  Str << v.reason_phrase + crfl;

  for (auto& header_pair : v.headers) {
    result += header_pair.first + ": " + header_pair.second + crfl;
  }
  result += crfl; //empty line to indicate end of header fields
  result += v.content;
  return Str << result;
}
