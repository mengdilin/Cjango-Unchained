#ifndef MyHttpRequestHeader
#define MyHttpRequestHeader
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include "http_session.hpp"
#include <pthread.h>

/** @file http_request.hpp
 * \ingroup http
 * @brief HttpRequest class declaration
 */

namespace http {
  class HttpRequest {
  public:
    std::string method;
    std::string path;
    std::string scheme;
  private:
    static unsigned long x, y, z; /*!< helper variables used to set session id */
    mutable unsigned long session_id;
    mutable bool has_set_session_id = false;
    std::unordered_map<std::string, std::string> meta; /*!< map that stores http request's header parameters */
    std::unordered_map<std::string, std::string> parameters; /*!< map that stores http request's get/post parameters */
    std::unordered_map<std::string, std::string> cookie;  /*!< map that stores http request's cookies */
    static std::unordered_map<std::string, std::shared_ptr<HttpSession> > sessions; /*!< static map that keeps track of all session objects for all http requests */
  public:
    static std::string session_cookie_key;
  public:
    HttpRequest(std::string path) : path(path) {}; // for easy testing
    HttpRequest(std::string,
      std::string, std::string,
      std::unordered_map<std::string, std::string>,
      std::unordered_map<std::string, std::string>,
      std::unordered_map<std::string, std::string>);
    ~HttpRequest();

  public:
    std::string get_method() const;
    std::string get_path() const;
    std::string get_scheme() const;
    unsigned long get_session_id();
    bool has_session_id();
    std::unordered_map<std::string, std::string> const & get_meta() const;
    std::unordered_map<std::string, std::string> const & get_parameters() const;
    std::unordered_map<std::string, std::string> const & get_cookie() const;
    std::shared_ptr<HttpSession> get_session();
    static unsigned long xorshf96(); //Marsaglia's xorshf generator


  };
  std::ostream& operator<<(std::ostream& Str, HttpRequest const & v);
}


#endif
