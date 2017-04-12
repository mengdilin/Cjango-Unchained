#include "http_request.hpp"
#include <stdio.h>      /* printf, NULL */
#include <stdlib.h>     /* strtoul */
#include "../app/externs.hpp"

unsigned long http::HttpRequest::x=123456789;
  unsigned long http::HttpRequest::y=362436069;
  unsigned long http::HttpRequest::z=521288629;
  std::string http::HttpRequest::session_cookie_key="session";
  std::unordered_map<std::string, std::unordered_map<std::string, std::string>*> http::HttpRequest::sessions;
http::HttpRequest::HttpRequest(
  std::string method,
  std::string path,
  std::string scheme,
  std::unordered_map<std::string, std::string> meta,
  std::unordered_map<std::string, std::string> params,
  std::unordered_map<std::string, std::string> cookie) {
  this->method = method;
  this->path = path;
  this->scheme = scheme;
  this->meta = meta;
  this->parameters = params;
  this->cookie = cookie;
}

http::HttpRequest::~HttpRequest() {

}
std::ostream& http::operator<<(std::ostream& Str, const http::HttpRequest& v) {
  std::string result = "method: " + v.get_method() + "\n"
            + "path: " + v.get_path()  + "\n"
            + "scheme: " + v.get_scheme() + "\n";
  result += "request headers: \n";
  for (auto& entry : v.get_meta()) {
    result  += entry.first + ":" + entry.second + "\n";
  }
  result += "parameters: \n";

  for (auto& entry : v.get_parameters()) {
    result  += entry.first + ":" + entry.second + "\n";
  }

  return Str << result;
}
//Marsaglia's xorshf generator
unsigned long http::HttpRequest::xorshf96() {          //period 2^96-1
  unsigned long t;
    HttpRequest::x ^= HttpRequest::x << 16;
    HttpRequest::x ^= HttpRequest::x >> 5;
    HttpRequest::x ^= HttpRequest::x << 1;
   t = HttpRequest::x;
   HttpRequest::x = HttpRequest::y;
   HttpRequest::y = HttpRequest::z;
   HttpRequest::z = t ^ HttpRequest::x ^ HttpRequest::y;

  return z;
}

std::string http::HttpRequest::get_method() const {
  return this->method;
}
std::string http::HttpRequest::get_path() const {
  return this->path;
}
std::string http::HttpRequest::get_scheme() const {
  return this->scheme;
}
std::unordered_map<std::string, std::string> const & http::HttpRequest::get_meta() const {
  return this->meta;
}
std::unordered_map<std::string, std::string> const & http::HttpRequest::get_parameters() const {
  return this->parameters;
}
std::unordered_map<std::string, std::string> const & http::HttpRequest::get_cookie() const {
  return this->cookie;
}

unsigned long http::HttpRequest::get_session_id() {
  return this->session_id;
}
bool http::HttpRequest::has_session_id() {
  return this->has_set_session_id;
}
std::unordered_map<std::string, std::string>* http::HttpRequest::get_session() {

  //TODO: r/w LOCKING session map here and in callbacks
  auto result = this->cookie.find(HttpRequest::session_cookie_key);
  for (auto it=this->cookie.begin(); it!=this->cookie.end(); ++it) {
    _DEBUG("cookie:  ", it->first, ",", it->second);
  }
  if (result != this->cookie.end()) {

    auto key = result->second;
    _DEBUG("found session key: ", key);
    _DEBUG("key equals 60441451194812: ", key=="60441451194812");

    auto session_result = HttpRequest::sessions.find(key);
    if (session_result != HttpRequest::sessions.end()) {
      return session_result->second;
    } else {
      _DEBUG("cannot find session id: ", key);
      unsigned long ul;
      ul = strtoul (key.c_str(), NULL, 0);
      this->has_set_session_id = true;
      this->session_id = ul;
      std::unordered_map<std::string, std::string>* map = new std::unordered_map<std::string, std::string>();
      HttpRequest::sessions.insert({key, map});
      return map;
    }


  } else {
    this->session_id = HttpRequest::xorshf96();
    this->has_set_session_id = true;
    _DEBUG("set session id to", std::to_string(this->session_id));
    _DEBUG("session id equals 60441451194812", std::to_string(this->session_id)=="60441451194812");
    std::unordered_map<std::string, std::string>* map = new std::unordered_map<std::string, std::string>();
    HttpRequest::sessions.insert({std::to_string(this->session_id), map});
    return map;
  }
}
