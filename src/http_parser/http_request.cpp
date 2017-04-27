#include "http_request.hpp"
#include <stdio.h>      /* printf, NULL */
#include <stdlib.h>     /* strtoul */
#include "../app/externs.hpp"

std::string http_logger_name = "http_request"; // FIXME refactor
  unsigned long http::HttpRequest::x=123456789; //FIXME: just use a static counter rather than generating this
  unsigned long http::HttpRequest::y=362436069;
  unsigned long http::HttpRequest::z=521288629;
  std::string http::HttpRequest::session_cookie_key="session";
  std::unordered_map<std::string, std::shared_ptr<http::HttpSession> > http::HttpRequest::sessions;

  static pthread_rwlock_t lock = PTHREAD_RWLOCK_INITIALIZER;

/**
** @brief HttpRequest Constructor
** @return void
*/
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

/**
** @brief HttpRequest toString method used for debug purposes
** @return void
*/
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

/**
** @brief Marsaglia's xorshf generator
** @return a pseudo random number
*/
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

/**
** @brief getter for http request's method. Ex: GET
*/
std::string http::HttpRequest::get_method() const {
  return this->method;
}

/**
** @brief getter for http request's path. Ex: /abc.html
*/
std::string http::HttpRequest::get_path() const {
  return this->path;
}

/**
** @brief getter for http request's scheme. Ex: HTTP/1.0
*/
std::string http::HttpRequest::get_scheme() const {
  return this->scheme;
}

/**
** @brief getter for http request's meta variables in the request header.
*/
std::unordered_map<std::string, std::string> const & http::HttpRequest::get_meta() const {
  return this->meta;
}

/**
** @brief getter for http request's get/post parameters
*/
std::unordered_map<std::string, std::string> const & http::HttpRequest::get_parameters() const {
  return this->parameters;
}

/**
** @brief getter for http request's cookie map
*/
std::unordered_map<std::string, std::string> const & http::HttpRequest::get_cookie() const {
  return this->cookie;
}

/**
** @brief getter for http request's session_id, persisted
** via cookie mechanism
*/
unsigned long http::HttpRequest::get_session_id() {
  return this->session_id;
}

/**
** @brief check if http request has a session_id associated with it
*/
bool http::HttpRequest::has_session_id() {
  return this->has_set_session_id;
}

/**
** @brief getter for a session object pointer associated with the
** current http request
*/
std::shared_ptr<http::HttpSession> http::HttpRequest::get_session() {

  auto result = this->cookie.find(HttpRequest::session_cookie_key);
  for (auto it=this->cookie.begin(); it!=this->cookie.end(); ++it) {
    //_SPDLOG(http_logger_name, info, "cookie:  {}, {}", it->first, it->second);
  }
  if (result != this->cookie.end()) {

    auto key = result->second;
    //_SPDLOG(http_logger_name, info, "found session key: {}", key);

    auto session_result = HttpRequest::sessions.find(key);
    if (session_result != HttpRequest::sessions.end()) {
      return session_result->second;
    } else {

        pthread_rwlock_wrlock(&lock);
        //re-check if another thread has added a new session
        //for the current key right after obtaining the lock
        auto session_result = HttpRequest::sessions.find(key);
        if (session_result != HttpRequest::sessions.end()) {
          pthread_rwlock_unlock(&lock);
          return session_result->second;
        } else {
          //_SPDLOG(http_logger_name, info, "cannot find session id: {}", key);
          unsigned long ul;
          ul = strtoul (key.c_str(), NULL, 0);
          this->has_set_session_id = true;
          this->session_id = ul;
          std::shared_ptr<HttpSession> new_session = std::make_shared<HttpSession>();
          HttpRequest::sessions.insert(std::pair<std::string, std::shared_ptr<HttpSession> >(key, new_session));
          pthread_rwlock_unlock(&lock);
          return new_session;
        }
    }


  } else {
    pthread_rwlock_wrlock(&lock);
    //re-check if another thread has added a new session
    //for the current key right after obtaining the lock
      this->session_id = HttpRequest::xorshf96();
      this->has_set_session_id = true;
      //_SPDLOG(http_logger_name, info, "set session id to {}", std::to_string(this->session_id));
      std::shared_ptr<HttpSession> new_session = std::make_shared<HttpSession>();

      HttpRequest::sessions.insert(std::pair<std::string, std::shared_ptr<HttpSession> >(std::to_string(this->session_id), new_session));
      pthread_rwlock_unlock(&lock);
      return new_session;
    }


}
