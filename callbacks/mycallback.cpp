#include <cjango>
#include <unordered_map>
extern "C" http::HttpResponse callback_hello_world(http::HttpRequest request) {
  return http::HttpResponse("helloWorld");
}

extern "C" http::HttpResponse callback_mine2(http::HttpRequest request) {
  _DEBUG("Hi, I'm dynamic callback");
  string text = "<html>\r\n<body>\r\n"
                "<h1>Hi there!</h1>\r\n"
                "<h1>This is just a DYNAMIC response version 2 :D</h1>\r\n"
                "<h1>Hi there!</h1>\r\n"
                "</body>\r\n</html>";
  http::HttpResponse resp(text);
  return resp;
}

extern "C" http::HttpResponse callback_mine3(http::HttpRequest request) {
  _DEBUG("Hi, I'm dynamic callback");
  string text = "<html>\r\n<body>\r\n"
                "<h1>Hi there!</h1>\r\n"
                "<h1>Hi there!</h1>\r\n"
                "<h1>This is just a Columbia response version 3 :) </h1>\r\n"
                "<h1>Hi there!</h1>\r\n"
                "<h1>Hi there!</h1>\r\n"
                "</body>\r\n</html>";
  http::HttpResponse resp(text);
  // return resp;
  return http::HttpResponse::render_to_response("doc/logo.png", "image/png", request);
}

std::string logger_name = "session"; // use both in page_index() and page_home()

extern "C" http::HttpResponse page_index(http::HttpRequest request) {
  //return http::HttpResponse("blah");

  auto session_map = request.get_session();

  session_map->set("user","mengdi");
  _SPDLOG(logger_name, info, "index session id: {}", std::to_string(request.get_session_id()));

  auto username = session_map->get("username");
  std::string response_body = "<html><p>welcome to your homepage, ";
  if (username != "") {
    response_body += username;
  }
  response_body += "</p></html>";
  //auto resp = http::HttpResponse::render_to_response("callbacks/templates/index.html", request);
  auto resp = http::HttpResponse(response_body, request);
  _DEBUG(resp.to_string());
  return resp;
}
// current_datetime() mimics the django example of
// https://docs.djangoproject.com/en/1.10/topics/http/views/

extern "C" http::HttpResponse page_home(http::HttpRequest request) {

  if (request.get_method() == "GET") {
    auto session_map = request.get_session();
  //session_map.insert({"user","mengdi"});
  _SPDLOG(logger_name, info, "index session id: {}", std::to_string(request.get_session_id()));

    return http::HttpResponse::render_to_response("../templates/index.html", request);
  } else if (request.get_method() == "POST"){
    _SPDLOG(logger_name, info, "home session id: {}", std::to_string(request.get_session_id()));
    auto session_map = request.get_session();
    auto params = request.get_parameters();
    auto first_name_result = params.find("firstname");
    std::string response_body = "<html><p>hello! ";
    if (first_name_result != params.end()) {
      response_body += first_name_result->second;
    }
    auto last_name_result = params.find("lastname");
    if (last_name_result != params.end()) {
      response_body += last_name_result->second;
    }
    response_body += "</p></html>";

    if (first_name_result != params.end()) {
      _SPDLOG(logger_name, info, "found first name: {}", first_name_result->second);
      session_map->set("username", first_name_result->second);
    }
    auto resp = http::HttpResponse(response_body, request);

    _SPDLOG(logger_name, info, "{}", resp.to_string());
    return resp;
  } else {
    return http::HttpResponse("<html><p>Not Supported Request: " + request.get_method());
  }
}
