#include "http_session.hpp"

http::HttpSession::~HttpSession() {

}


http::HttpSession::HttpSession() {
  //initializes rw lock
  pthread_rwlock_init(&lock, NULL);
}

/**
** @brief exception class in case HttpSession::get wants to hard-fail when key is not present
*/
class http_session_get_exception: public std::exception {
  virtual const char* what() const throw()
  {
    return "cannot find key in session";
  }
} sessionex;


/**
** @brief a key in a session map, return its value. Returns an empty string if key not found
*/
std::string http::HttpSession::get(std::string key) {

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

/**
** @brief a (key, value pair), insert it to session map
*/
void http::HttpSession::set(std::string key, std::string value) {
  //get exclusive access for write lock
  pthread_rwlock_wrlock(&lock);
  auto result = single_session.find(key);
  if (result != single_session.end()) {
    result->second = value;
  } else {
    single_session.insert(std::pair<std::string, std::string>(key, value));
  }
  pthread_rwlock_unlock(&lock);
}
