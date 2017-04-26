#include <catch.hpp>
#include "../http_parser/http_request.hpp"
#include "../http_parser/http_request_parser.hpp"
#include <sstream>
#include <unordered_map>

TEST_CASE("HttpRequestParser - request parsing", "[HttpRequestParser]") {
  std::string get_test = "GET /path/file.html HTTP/1.0\r\nUser-Agent:HTTPTool/1.0\r\n\r\n";
  http::HttpRequestParser parser;
  std::istringstream ss(get_test);
  http::HttpRequest headers = parser.parse_request_line_and_headers(ss);

  REQUIRE( headers.get_method() == "GET" );
  REQUIRE( headers.get_path() == "/path/file.html" );
  REQUIRE( headers.get_scheme() == "HTTP/1.0" );
  REQUIRE( headers.get_scheme() == "HTTP/1.0" );
  auto result = headers.get_meta().find("User-Agent");
  REQUIRE( result != headers.get_meta().end() );
  REQUIRE( result->second == "HTTPTool/1.0" );
}

TEST_CASE("HttpRequestParser - get param request parsing", "[HttpRequestParser]") {
  std::string get_test = "GET /abc/?abc=aa&ee=f  HTTP/1.0\r\nUser-Agent:HTTPTool/1.0\r\n\r\n";
  http::HttpRequestParser parser;
  std::istringstream ss(get_test);
  http::HttpRequest headers = parser.parse_request_line_and_headers(ss);

  REQUIRE( headers.get_method() == "GET" );
  REQUIRE( headers.get_path() == "/abc/" );
  REQUIRE( headers.get_scheme() == "HTTP/1.0" );
  REQUIRE( headers.get_scheme() == "HTTP/1.0" );
  auto result = headers.get_meta().find("User-Agent");
  REQUIRE( result != headers.get_meta().end() );
  REQUIRE( result->second == "HTTPTool/1.0" );
  auto params = headers.get_parameters();
  auto param_result = params.find("abc");
  REQUIRE( param_result != params.end() );
  REQUIRE( param_result->second == "aa" );
}

TEST_CASE("HttpRequestParser - post param request parsing", "[HttpRequestParser]") {
  std::string get_test = "POST /cgi-bin/process.cgi HTTP/1.1\r\nContent-Type: text/xml; charset=utf-8; application/x-www-form-urlencoded\r\nContent-Length: 100\r\n\r\n"
  "licenseID=string&content=string&/paramsXML=string\r\n";

  http::HttpRequestParser parser;
  std::istringstream ss(get_test);
  http::HttpRequest headers = parser.parse(ss);

  REQUIRE( headers.get_method() == "POST" );
  REQUIRE( headers.get_path() == "/cgi-bin/process.cgi" );
  REQUIRE( headers.get_scheme() == "HTTP/1.1" );
  auto result = headers.get_meta().find("Content-Type");
  REQUIRE( result != headers.get_meta().end() );
  REQUIRE( result->second == " text/xml; charset=utf-8; application/x-www-form-urlencoded" );
  auto params = headers.get_parameters();
  REQUIRE(params.size() != 0);
  auto param_result = params.find("licenseID");
  REQUIRE( param_result != params.end() );
  REQUIRE( param_result->second == "string" );
  param_result = params.find("content");
  REQUIRE( param_result != params.end() );
  REQUIRE( param_result->second == "string" );
  param_result = params.find("/paramsXML");
  REQUIRE( param_result != params.end() );
}
