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
  //http::HttpResponse resp(text);
  //return resp;
  return http::HttpResponse::render_to_response("logo.png", "image/png", request);
}

extern "C" http::HttpResponse page_index(http::HttpRequest request) {
  //return http::HttpResponse("blah");

  auto session_map = request.get_session();
  session_map->insert({"user","mengdi"});
  _DEBUG("index session id: ", std::to_string(request.get_session_id()));
for (auto it=session_map->begin(); it!=session_map->end(); ++it) {
      _DEBUG("session:  ", it->first, ",", it->second);
    }
  auto username = session_map->find("username");
  std::string response_body = "<html><p>welcome to your homepage, ";
  if (username != session_map->end()) {
    response_body += username->second;
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
  _DEBUG("index session id: ", std::to_string(request.get_session_id()));
for (auto it=session_map->begin(); it!=session_map->end(); ++it) {
      _DEBUG("session:  ", it->first, ",", it->second);
    }
    return http::HttpResponse::render_to_response("callbacks/templates/index.html", request);
  } else if (request.get_method() == "POST"){
    _DEBUG("home session id: ", std::to_string(request.get_session_id()));
    auto session_map = request.get_session();
    auto params = request.get_parameters();
    auto first_name_result = params.find("firstname");
    std::string response_body = "<html><p>hello! ";
    _DEBUG("HERE1");
    if (first_name_result != params.end()) {
      response_body += first_name_result->second;
    }
    _DEBUG("HERE2");
    auto last_name_result = params.find("lastname");
    if (last_name_result != params.end()) {
      response_body += last_name_result->second;
    }
    _DEBUG("HERE3");
    response_body += "</p></html>";

    _DEBUG("HERE4");
    if (first_name_result != params.end()) {
      _DEBUG("found first name", first_name_result->second);
      session_map->insert({"username", first_name_result->second});
    }
    for (auto it=session_map->begin(); it!=session_map->end(); ++it) {
      _DEBUG("before session:  ", it->first, ",", it->second);
    }
    session_map = request.get_session();
    for (auto it=session_map->begin(); it!=session_map->end(); ++it) {
      _DEBUG("after session:  ", it->first, ",", it->second);
    }
      auto resp = http::HttpResponse(response_body, request);

    _DEBUG(resp.to_string());
    return resp;
  } else {
    return http::HttpResponse("<html><p>Not Supported Request: " + request.get_method());
  }
}
