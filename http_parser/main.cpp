#include "http_request_parser.hpp"
#include "http_response.hpp"
#include <sstream>
#include <unordered_map>
int main() {
  std::string header = "POST /cgi-bin/process.cgi HTTP/1.1\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nHost: www.tutorialspoint.com\r\nContent-Type: text/xml; charset=utf-8\r\nContent-Length: length\r\nAccept-Language: en-us\r\nAccept-Encoding: gzip, deflate\r\nConnection: Keep-Alive\r\n\r\nlicenseID=string&content=string&/paramsXML=string\r\n";
  std::string get_test = "GET /path/file.html HTTP/1.0\r\nFrom: someuser@jmarshall.com\r\nUser-Agent: HTTPTool/1.0\r\n\r\n";
  std::string nc_get = "GET /abc/?abc=aa&ee=f HTTP/1.1\r\nHost: localhost:5000\r\nConnection: keep-alive\r\nUpgrade-Insecure-Requests: 1\r\nCookie: tasty_cookie=strawberry; hello=123\r\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_4) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/57.0.2987.133 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\nDNT: 1\r\nAccept-Encoding: gzip, deflate, sdch, br\r\nAccept-Language: en-US,en;q=0.8,zh-CN;q=0.6,zh;q=0.4,zh-TW;q=0.2";
    std::cout << nc_get << std::endl;

  try {
    http::HttpRequestParser parser;
  std::istringstream ss(nc_get);
  //std::cout << ss << std::endl;
  http::HttpRequest headers = parser.parse_request_line_and_headers(ss);
  std::cout << "left over leng: " << ss.str().length() << std::endl;
  std::cout << "tellg(): " << ss.tellg() << std::endl;
  std::unordered_map<std::string, std::string> map = parser.parse_body(ss, "application/x-www-form-urlencoded", ss.str().length());

  //std::unordered_map<std::string, std::string> body = parser.parse_body(ss, );
  std::cout << headers << std::endl;
  std::cout << "finished headers" << std::endl;
    for (auto entry : map) {
    std::cout << entry.first << ":" << entry.second << std::endl;
  }
  std::cout << "finished cookies" << std::endl;

  for (auto entry : headers.get_cookie()) {
        std::cout << entry.first << ":" << entry.second << std::endl;

  }
  std::cout << "here" << std::endl;
  http::HttpResponse response = http::HttpResponse::render_to_response("../test/index.html");

  std::cout << response << std::endl;
} catch (const char *e) {

  std::cout << e << std::endl;
}
return 1;
}
