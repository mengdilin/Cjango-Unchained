#include "http_request_parser.hpp"
#include <sstream>
#include <unordered_map>
int main() {
  std::string header = "POST /cgi-bin/process.cgi HTTP/1.1\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nHost: www.tutorialspoint.com\r\nContent-Type: text/xml; charset=utf-8\r\nContent-Length: length\r\nAccept-Language: en-us\r\nAccept-Encoding: gzip, deflate\r\nConnection: Keep-Alive\r\n\r\n";

  try {
    http::HttpRequestParser parser;
  std::istringstream ss(header);
  //std::cout << ss << std::endl;
  http::HttpRequest headers = parser.parse_request_line_and_headers(ss);
  //std::unordered_map<std::string, std::string> body = parser.parse_body(ss, );
  std::cout << headers << std::endl;
} catch (char *e) {

  std::cout << e << std::endl;
}
return 1;
}
