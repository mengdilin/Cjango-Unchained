#ifndef MyHttpSessionHeader
#define MyHttpSessionHeader
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <string>
#include <mutex>  // For std::unique_lock
#include <exception>
#include <pthread.h>
/** @file http_session.hpp
 * \ingroup http
 * @brief HttpSession class declaration
 */

namespace http {
  class HttpSession {
  private:
    std::unordered_map<std::string, std::string> single_session;
    //mutable ting::shared_mutex mutex_;
    pthread_rwlock_t lock;

  public:
    HttpSession();
    ~HttpSession();
    std::string get(std::string);
    void set(std::string, std::string);
  };
}
#endif
