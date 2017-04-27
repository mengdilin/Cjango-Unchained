#include "http_request_line.hpp"

/** @file http_request_line.cpp
 * \ingroup http
 * @brief HttpRequestLine class implementation
 */

/**
* @brief HttpRequestLine constructor that represents
* the first line of a http request as well as the paramters
* of a get HttpRequest
* @param action: method of a http request
* @param uri: path of a http request
* @param parameters: a map containing the parameters of a http get request
* @param protocolVersion: version of the http protocol for the request
*/
http::HttpRequestLine::HttpRequestLine(std::string action, std::string uri, std::string protocolVersion, std::unordered_map<std::string, std::string> parameters) {
      this->action = action;
      this->uri = uri;
      this->parameters = parameters;
      this->protocolVersion = protocolVersion;
}

/**
* @brief HttpRequestLine constructor that represents
* the first line of a http request
* @param action: method of a http request
* @param uri: path of a http request
* @param protocolVersion: version of the http protocol for the request
*/
http::HttpRequestLine::HttpRequestLine(std::string action, std::string uri, std::string protocolVersion) {
      this->action = action;
      this->uri = uri;
      this->protocolVersion = protocolVersion;
}
