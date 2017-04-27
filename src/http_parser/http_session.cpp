#include "http_session.hpp"

/** @file http_session.cpp
 * \ingroup http
 * @brief HttpSession class implementation
 */


http::HttpSession::~HttpSession() {

}

/**
* @brief HttpSession constructor that initializes a reader-writer lock
*/
http::HttpSession::HttpSession() {
  //initializes rw lock
  pthread_rwlock_init(&lock, NULL);
}

/**
** @brief custom exception class in case
** HttpSession::get wants to hard-fail when key is not present
*/
class http_session_get_exception: public std::exception {
  virtual const char* what() const throw()
  {
    return "cannot find key in session";
  }
} sessionex;


/**
** @brief given a key, return its value from the session map.
** This method is thread safe
** @param key: key used to retrieve value stored in the session map
** @return if key is found, the value corresponding to the key.
** If key is not found, an empty string
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
** @brief given a (key, value pair), insert it to the session map.
** This method is thread safe
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
