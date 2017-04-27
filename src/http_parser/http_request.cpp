#include "http_request.hpp"
#include <stdio.h>      /* printf, NULL */
#include <stdlib.h>     /* strtoul */
#include "../app/externs.hpp"

/** @file http_request.cpp
 * \ingroup http
 * @brief HttpRequest class implementation
 */
std::string http_logger_name = "http_request"; // FIXME refactor
  unsigned long http::HttpRequest::x=123456789; //FIXME: just use a static counter rather than generating this
  unsigned long http::HttpRequest::y=362436069;
  unsigned long http::HttpRequest::z=521288629;
  std::string http::HttpRequest::session_cookie_key="session";
  std::unordered_map<std::string, std::shared_ptr<http::HttpSession> > http::HttpRequest::sessions;

  static pthread_rwlock_t lock = PTHREAD_RWLOCK_INITIALIZER;

/**
* @brief HttpRequest Constructor that represents one HTTP request that can be received by a server
* @param method: http request method
* @param path: http request path
* @param scheme: http request version
* @param meta: a map containing the key value pairs of http headers
* @param params: a map containing the key value pairs of http parameters
* @param cookie: a map containing the key value pairs of http header's cookie entries
* @return HttpRequest
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
** @brief overloading << for HttpRequest object that enables HttpRequest to be written to a output stream
** @return std::ostream& enabling HttpRequest to be written to a output stream
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
** @return a pseudo random unsigned long
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
** @return a string indicating http request's method
*/
std::string http::HttpRequest::get_method() const {
  return this->method;
}

/**
** @brief getter for http request's path. Ex: /abc.html
** @return a path string
*/
std::string http::HttpRequest::get_path() const {
  return this->path;
}

/**
** @brief getter for http request's scheme. Ex: HTTP/1.0
** @return a string indicating the current http protocol version of the request
*/
std::string http::HttpRequest::get_scheme() const {
  return this->scheme;
}

/**
** @brief getter for http request's meta variables in the request header.
** @return a map of key value pairs for http request header
*/
std::unordered_map<std::string, std::string> const & http::HttpRequest::get_meta() const {
  return this->meta;
}

/**
** @brief getter for http request's get/post parameters
** @return a map of key value pairs for http request parameters
*/
std::unordered_map<std::string, std::string> const & http::HttpRequest::get_parameters() const {
  return this->parameters;
}

/**
** @brief getter for http request's cookie map
** @return a map of key value pairs for http request cookie
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
** @brief getter for a session object pointer associated with the current http request
** If there is no HttpSession object associated with the http request, a new HttpSession
** object will be created for the request.
** @return a shared_ptr of HttpSession object
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
