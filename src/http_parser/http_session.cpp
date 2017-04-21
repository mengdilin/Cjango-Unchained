#include "http_session.hpp"

http::HttpSession::~HttpSession() {

}


http::HttpSession::HttpSession() {

}

class http_session_get_exception: public std::exception {
  virtual const char* what() const throw()
  {
    return "cannot find key in session";
  }
} sessionex;


std::string http::HttpSession::get(std::string key) {
  //get shared access for read lock
  std::shared_lock<std::shared_mutex> lock(mutex_);
  auto result = single_session.find(key);
  if (result != single_session.end()) {
    return result->second;
  } else {
    return "";
    //throw sessionex;
    //soft failure when key not found?
  }

}

void http::HttpSession::set(std::string key, std::string value) {
  //get exclusive access for write lock
  std::unique_lock<std::shared_mutex> lock(mutex_);
  auto result = single_session.find(key);
  if (result != single_session.end()) {
    result->second = value;
  } else {
    single_session.insert({key, value});
  }
}
