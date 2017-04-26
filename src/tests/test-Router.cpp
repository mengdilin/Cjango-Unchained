#include <catch.hpp>
#include "../routing/router.hpp"

functor f_router_test = [](http::HttpRequest){ return http::HttpResponse("test"); };

TEST_CASE("Router - path resolve() and get_http_response()", "[router]") {
  Router r;
  std::string url_pattern("/abc");
  r.add_route(url_pattern, f_router_test);

  http::HttpRequest req(url_pattern);
  REQUIRE( r.resolve(req) == url_pattern );
  REQUIRE( r.get_http_response(req).to_string() == http::HttpResponse("test").to_string() );

}

TEST_CASE("static_dir setting") {
  Router r;
  const std::string sdir = "/custom-static";
  r.set_static_dir(sdir);
  REQUIRE( r.get_static_dir() == sdir );

  http::HttpRequest req(sdir + "/photo.jpg");
  try {
    std::string s = r.resolve(req);
  } catch (RouterException e) {
    REQUIRE( e == RouterException::STATIC_FILE_SERVED );
  }

  http::HttpRequest req2("/some_invalid_url");
  try {
    std::string s = r.resolve(req2);
  } catch (RouterException e) {
    REQUIRE( e == RouterException::INVALID_URL );
  }
}

TEST_CASE("size of URL pattern map") {
  Router r;
  std::string url_pattern("/abc");
  r.add_route(url_pattern, f_router_test);

  REQUIRE( r.nr_patterns() == 1 );

  /*
   * SECTION is expanded into if statement. And interestingly,
   * each SECTION is executed from the start of this TEST_CASE statement.
   * In other words, the statements before each SECTION is
   * essentially common setups for all SECTIONs.
   * For more info: https://github.com/philsquared/Catch/blob/master/docs/tutorial.md#test-cases-and-sections
   */

  SECTION("when adding one more URL map") {
    r.add_route("/second_rule", f_router_test);
    REQUIRE( r.nr_patterns() == 2 );
  }

  SECTION("this section still has one rule") {
    REQUIRE( r.nr_patterns() == 1 );
  }

  SECTION("this rule is the already-added one") {
    r.add_route(url_pattern, f_router_test);
    REQUIRE( r.nr_patterns() == 1 ); // no increase
  }
}

TEST_CASE("delete all patterns") {
  Router r;
  r.add_route("/1", f_router_test);
  r.add_route("/2", f_router_test);
  r.add_route("/3", f_router_test);

  REQUIRE( r.nr_patterns() == 3 );

  r.erase_all_patterns();
  REQUIRE( r.nr_patterns() == 0 );
}