#ifndef MyHttpRequestHeader
#define MyHttpRequestHeader
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <string>
/** @file http_request.hpp
 * \ingroup http
 * @brief HttpRequest class declaration
 */

namespace http {
  class HttpRequest {
  private:
    std::string method;
    static unsigned long x, y, z;
    mutable unsigned long session_id;
    mutable bool has_set_session_id = false;

    std::string path;
    std::string scheme;
    std::unordered_map<std::string, std::string> meta;
    std::unordered_map<std::string, std::string> parameters;
    std::unordered_map<std::string, std::string> cookie;
    static std::unordered_map<std::string, std::unordered_map<std::string, std::string>*> sessions;
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
    std::unordered_map<std::string, std::string>* get_session();
    static unsigned long xorshf96(); //Marsaglia's xorshf generator


  };
  std::ostream& operator<<(std::ostream& Str, HttpRequest const & v);
  //std::string HttpRequest::session_cookie_key="session";
}


#endif
