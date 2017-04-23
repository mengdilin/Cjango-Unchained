#include "http_session.hpp"

http::HttpSession::~HttpSession() {

}


http::HttpSession::HttpSession() {
  pthread_rwlock_init(&lock, NULL);
}

class http_session_get_exception: public std::exception {
  virtual const char* what() const throw()
  {
    return "cannot find key in session";
  }
} sessionex;


std::string http::HttpSession::get(std::string key) {
  //get shared access for read lock
  //std::shared_lock<ting::shared_mutex> lock(mutex_);
  pthread_rwlock_rdlock(&lock);
  auto result = single_session.find(key);
  if (result != single_session.end()) {
    pthread_rwlock_unlock(&lock);
    return result->second;
  } else {
    pthread_rwlock_unlock(&lock);
    return "";
    //throw sessionex;
    //soft failure when key not found?
  }

}

void http::HttpSession::set(std::string key, std::string value) {
  //get exclusive access for write lock
  //std::unique_lock<ting::shared_mutex> lock(mutex_);
  pthread_rwlock_wrlock(&lock);
  auto result = single_session.find(key);
  if (result != single_session.end()) {
    result->second = value;
  } else {
    single_session.insert({key, value});
  }
  pthread_rwlock_unlock(&lock);
}
