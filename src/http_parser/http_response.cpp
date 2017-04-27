#include "http_response.hpp"
#include <fstream>
#include <streambuf>
#include "../app/externs.hpp"
std::string http::HttpResponse::templates_root;

/** @file http_response.cpp
 * \ingroup http
 * @brief HttpResponse class implementation
 */

/**
** @brief given a key value pair, inserts the pair to request's cookie
** @param key: cookie key
** @param value: cookie value
*/
void http::HttpResponse::set_cookie(std::string key, std::string value) {
  auto result = headers.find("Set-Cookie");
  if (result != headers.end()) {
    //_DEBUG("inside first");
    result->second += "; "+key+"="+value;
    headers.insert(std::pair<std::string, std::string>("Set-Cookie", result->second));

  } else {
    headers.insert(std::pair<std::string, std::string>("Set-Cookie", key+"="+value));
  }

}

/**
** @brief given a file path, content type of the file, and a http request,
** create a http response
** @param path: path to a file whose content will be in the http response's body.
** path should be relative to HttpResponse::templates_root
** @param content_type: content type of the http response
** whose value is set in http response's header
** @param request: a HttpRequest object which corresponds to the current http response
** @return HttpResponse
*/
http::HttpResponse http::HttpResponse::render_to_response(std::string path, std::string content_type, http::HttpRequest& request) {
  auto response = HttpResponse::render_to_response(path, content_type);
  if (request.has_session_id()) {
    response.set_cookie(HttpRequest::session_cookie_key, std::to_string(request.get_session_id()));
  }
  return response;
}

/**
** @brief given a file path and a http request, generate a http response.
** The generated HttpResponse will have text/html as its content-type
** and a status code of 200.
** @param path: path to a file whose content will be in the http response's body.
** path should be relative to HttpResponse::templates_root
** @param request: a HttpRequest object which corresponds to the current http response
** @return HttpResponse
*/
http::HttpResponse http::HttpResponse::render_to_response(std::string file, http::HttpRequest& request) {
  auto response = HttpResponse::render_to_response(file);
  if (request.has_session_id()) {
    response.set_cookie(HttpRequest::session_cookie_key, std::to_string(request.get_session_id()));
  }
  return response;
}

/**
** @brief given a file path, generate a http response.
** The generated HttpResponse will have text/html as its content-type
** and a status code of 200.
** @param path: path to a file whose content will be in the http response's body.
** path should be relative to HttpResponse::templates_root
** @return HttpResponse
*/
http::HttpResponse http::HttpResponse::render_to_response(std::string path) {
  std::ifstream t(HttpResponse::templates_root+path);
  std::string str((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());
  return HttpResponse(str);
}


/**
** @brief given a file path, generate a string containing file's data
** @param path: path to a file whose content will be returned as a string
** path should be relative to HttpResponse::templates_root
** @return a string representing the file's content
*/
std::string http::HttpResponse::get_template(std::string path) {
  std::ifstream t(HttpResponse::templates_root+path);
  std::string str((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());
  return str;
}

/**
** @brief given a file path and content type of the file, generate a http response
** The generated HttpResponse will have text/html as its content-type
** and a status code of 200.
** @param path: path to a file whose content will be in the http response's body.
** path should be relative to HttpResponse::templates_root
** @param content_type: content type of the http response
** whose value is set in http response's header
** @return HttpResponse
*/
http::HttpResponse http::HttpResponse::render_to_response(std::string path, std::string content_type) {
  std::ifstream t(HttpResponse::templates_root+path);
  std::string str((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());
  return HttpResponse(str, content_type);
}

//default good http response
/**
** @brief HttpResponse constructor with a 200 status code
** @param content: body of the http response
** @param content_type: content type of the http response
** @return HttpResponse
*/
http::HttpResponse::HttpResponse(std::string content, std::string content_type) {
  this->content = content;
  this->content_type = content_type;
  headers.insert(std::pair<std::string, std::string>("Content-Type", content_type));
  headers.insert(std::pair<std::string, std::string>("Content-Length", std::to_string(content.length())));
}

//default good http response
/**
** @brief HttpResponse constructor with a 200 status code
** @param content: body of the http response
** @return HttpResponse
*/
http::HttpResponse::HttpResponse(std::string content) {
  this->content = content;
  headers.insert(std::pair<std::string, std::string>("Content-Type", content_type));
  headers.insert(std::pair<std::string, std::string>("Content-Length", std::to_string(content.length())));
}

//default good http response
/**
** @brief HttpResponse constructor with a 200 status code and content type text/html
** @param content: body of the http response
** @param request: a HttpRequest object which corresponds to the current http response
** @return HttpResponse
*/
http::HttpResponse::HttpResponse(std::string content, http::HttpRequest& request) {
  this->content = content;
  headers.insert(std::pair<std::string, std::string>("Content-Type", content_type));
  headers.insert(std::pair<std::string, std::string>("Content-Length", std::to_string(content.length())));
  if (request.has_session_id()) {
    this->set_cookie(HttpRequest::session_cookie_key, std::to_string(request.get_session_id()));
  }
}

/**
** @brief helper method that translates http status code to status message
** @param status_code: valid http response status code defined in http/1.0 protocol
** @return the corresponding status message
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
/**
** @brief HttpResponse constructor with content type text/html
** @param status_code: status code of the http response
** @return HttpResponse
*/
http::HttpResponse::HttpResponse(int status_code) {
  this->status_code = status_code;
  get_reason_phrase(status_code);
  headers.insert(std::pair<std::string, std::string>("Content-Type", content_type));
  headers.insert(std::pair<std::string, std::string>("Content-Length", std::to_string(content.length())));
}

/**
** @brief returns a well-formated string version of http response
** compliant with http/1.0 protocol
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
** @brief overloads << function for debugging purposes: convenience of printing out http response
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
