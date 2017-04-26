#include <cjango>

extern "C" http::HttpResponse return_hello_json(http::HttpRequest request) {

  //  an example json in https://stedolan.github.io/jq/tutorial/
  //  URLs are shortened for demo purpose
  std::string json_str(
  "{\"parents\": ["
    "{                                                                   "
    "  \"sha\": \"54b9c9bdb225af5d886466d72f47eafc51acb4f7\",            "
    "  \"url\": \"https://api.github.com/repos/stedolan/jq/commits/54b\","
    "  \"html_url\": \"https://github.com/stedolan/jq/commit/54b\"       "
    "},                                                                  "
    "{                                                                   "
    "  \"sha\": \"8b1b503609c161fea4b003a7179b3fbb2dd4345a\",            "
    "  \"url\": \"https://api.github.com/repos/stedolan/jq/commits/8b1\","
    "  \"html_url\": \"https://github.com/stedolan/jq/commit/8b1\"       "
    "} ]                                                                 "
  "}");

  // std::string.c_str() converts a C++ string into a C-language string
  return http::HttpResponse(json_str.c_str(), "application/json");
}
