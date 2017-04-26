#include "http_response.hpp"
#include <fstream>
#include <streambuf>
#include "../app/externs.hpp"
std::string http::HttpResponse::templates_root;

/**
** @brief given a key value pair, inserts the pair to request's cookie
*/
void http::HttpResponse::set_cookie(std::string key, std::string value) {
  auto result = headers.find("Set-Cookie");
  if (result != headers.end()) {
    _DEBUG("inside first");
    result->second += "; "+key+"="+value;
    headers.insert({"Set-Cookie", result->second});

  } else {
    headers.insert({"Set-Cookie", key+"="+value});
  }

}

/**
** @brief given a file path, content type of the file, and a http request, generate a http response
*/
http::HttpResponse http::HttpResponse::render_to_response(std::string path, std::string content_type, http::HttpRequest& request) {
  auto response = HttpResponse::render_to_response(path, content_type);
  if (request.has_session_id()) {
    response.set_cookie(HttpRequest::session_cookie_key, std::to_string(request.get_session_id()));
  }
  return response;
}

/**
** @brief given a file path and a http request, generate a http response
*/
http::HttpResponse http::HttpResponse::render_to_response(std::string file, http::HttpRequest& request) {
  auto response = HttpResponse::render_to_response(file);
  if (request.has_session_id()) {
    response.set_cookie(HttpRequest::session_cookie_key, std::to_string(request.get_session_id()));
  }
  return response;
}

/**
** @brief given a file path, generate a http response
*/
http::HttpResponse http::HttpResponse::render_to_response(std::string path) {
  std::ifstream t(HttpResponse::templates_root+path);
  std::string str((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());
  return HttpResponse(str);
}


/**
** @brief given a file path, generate a string containing file's data
*/
std::string http::HttpResponse::get_template(std::string path) {
  std::ifstream t(HttpResponse::templates_root+path);
  std::string str((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());
  return str;
}

/**
** @brief given a file path and content type of the file, generate a http response
*/
http::HttpResponse http::HttpResponse::render_to_response(std::string path, std::string content_type) {
  std::ifstream t(HttpResponse::templates_root+path);
  std::string str((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());
  return HttpResponse(str, content_type);
}
//default good http response
http::HttpResponse::HttpResponse(std::string content, std::string content_type) {
  this->content = content;
  this->content_type = content_type;
  headers.insert({"Content-Type", content_type});
  headers.insert({"Content-Length", std::to_string(content.length())});
}

//default good http response
http::HttpResponse::HttpResponse(std::string content) {
  this->content = content;
  headers.insert({"Content-Type", content_type});
  headers.insert({"Content-Length", std::to_string(content.length())});
}

//default good http response
http::HttpResponse::HttpResponse(std::string content, http::HttpRequest& request) {
  this->content = content;
  headers.insert({"Content-Type", content_type});
  headers.insert({"Content-Length", std::to_string(content.length())});
  if (request.has_session_id()) {
    this->set_cookie(HttpRequest::session_cookie_key, std::to_string(request.get_session_id()));
  }
}

/**
** @brief helper method that translates http status code to status message
*/
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

//default good constructor
http::HttpResponse::HttpResponse(int status_code) {
  this->status_code = status_code;
  get_reason_phrase(status_code);
  headers.insert({"Content-Type", content_type});
  headers.insert({"Content-Length", std::to_string(content.length())});
}

/**
** @brief returns a well-formated string version of http response compliant with the http protocol
*/
std::string http::HttpResponse::to_string() {
  std::string result;
  std::string crfl = "\r\n";
  std::string sp = " ";
  result += http_version + sp + std::to_string(status_code) + sp + reason_phrase + crfl;
  result+= "Content-Type: " + this->content_type + crfl;
  for (auto& header_pair : headers) {
    if (header_pair.first != "Content-Type") {
      result += header_pair.first + ": " + header_pair.second + crfl;
    }

  }
  result += crfl;
  result += content;
  return result;
}
/**
** @brief helper function for debugging purposes: convenience of printing out http response
*/
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
