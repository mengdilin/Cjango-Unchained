#include <cjango>
#include <unordered_map>
#include <sqlite3.h>
#include <chrono>
#include <sstream>
#include <iomanip>
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

inline int sql_callback_insert(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

inline int sql_callback_select(void *data, int argc, char **argv, char **azColName){
   int i;
   std::string *pstr = static_cast<std::string *>(data);
   *(pstr) += "<tr>";
   for(i=0; i<argc; i++){

      *(pstr) += "<td>";
      *(pstr) += argv[i];
      *(pstr) += "</td>";


      //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   *(pstr) += "</tr>";
   //printf("\n");
   return 0;
}

inline std::string datetime_now() {
  auto now = std::chrono::system_clock::now();
  std::time_t now_clock = std::chrono::system_clock::to_time_t(now);
  std::stringstream ss;
  ss << std::put_time(std::localtime(&now_clock), "%c");
  return ss.str();
}

extern "C" http::HttpResponse page_insert(http::HttpRequest request) {
  //return http::HttpResponse("blah");
  _SPDLOG(logger_name, info, "index session id: {}", std::to_string(request.get_session_id()));
 std::string response_end_body = "<div><form action=\"/search_blog\" method=\"GET\"><input type=\"text\" name=\"search\" placeholder=\"query\"><input type=\"submit\" value=\"search\"></form></div>"
    "<div><form action=\"/insert_blog\" method=\"POST\"><textarea name=\"blog_post\" placeholder=\"write a blog...\"></textarea><input type=\"submit\" value=\"submit\"></form></div>"
    "</body></html>";
  auto session_map = request.get_session();
  std::string username = session_map->get("username");

  std::string response_body = "<html><body>";
  if (username != "") {
    response_body+="<p> welcome, " + username + "</p><br>";
  }

  auto wrote_blog = request.get_parameters().find("blog_post");
  std::string now = datetime_now();

  std::string blog;
  if (wrote_blog != request.get_parameters().end()) {

    blog = wrote_blog->second;
    _SPDLOG(logger_name, info, "found param: {}", blog);
    std::string sql = "INSERT INTO blogs (date, blog) VALUES (\"" + now + "\", \"" + blog + "\");";

    sqlite3 *db;
    int rc = sqlite3_open("./demoDb.db", &db);
    if (rc) {
      _SPDLOG(logger_name, info, "cannot open db: {}", "demoDB.db");
      return http::HttpResponse(response_body, request);
    }

    char *zErrMsg = 0;
    rc = sqlite3_exec(db, sql.c_str(), sql_callback_insert, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
      _SPDLOG(logger_name, info, "insert into db failed: {} with sql: {}", "demoDb.db", sql);
      _SPDLOG(logger_name, info, "sql {}: ", sql);
      _SPDLOG(logger_name, info, "error {}: ", zErrMsg);
      sqlite3_free(zErrMsg);
      response_body += "<p>error inserting into sql db </p><br>" + response_end_body;

      //auto resp = http::HttpResponse::render_to_response("callbacks/templates/index.html", request);
      auto resp = http::HttpResponse(response_body, request);
      return resp;
    }

    sql = "select * from blogs;";
    _SPDLOG(logger_name, info, "running sql query: {}", sql);
    std::string data = "<table border=\"1\"><tr><th>Date</th><th>Blog</th></tr>";
    rc = sqlite3_exec(db, sql.c_str(), sql_callback_select, (void *)&data, &zErrMsg);
    if( rc != SQLITE_OK ){
      _SPDLOG(logger_name, info, "select from db failed: {} with sql: {}", "demoDb.db", sql);
      _SPDLOG(logger_name, info, "sql {}: ", sql);
      _SPDLOG(logger_name, info, "error {}: ", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {

      data += "</table>";
      response_body += "<body>" + data + "<br>";
      _SPDLOG(logger_name, info, "result: {}", data);
    }
    sqlite3_close(db);
  } else {
    _SPDLOG(logger_name, info, "cannot find param: {}", "query");
  }


  response_body += response_end_body;

  //auto resp = http::HttpResponse::render_to_response("callbacks/templates/index.html", request);
  auto resp = http::HttpResponse(response_body, request);
  _DEBUG(resp.to_string());



  return resp;
}
extern "C" http::HttpResponse page_index(http::HttpRequest request) {
  //return http::HttpResponse("blah");
  _SPDLOG(logger_name, info, "index session id: {}", std::to_string(request.get_session_id()));

  auto session_map = request.get_session();
  std::string username = session_map->get("username");

  std::string response_body = "<html><body>";
  if (username != "") {
    response_body+="<p> welcome, " + username + "</p><br>";
  }
  auto query = request.get_parameters().find("search");
  std::string query_string;
  if (query != request.get_parameters().end()) {

    query_string = query->second;
    _SPDLOG(logger_name, info, "found param: {}", query_string);
    sqlite3 *db;
    int rc = sqlite3_open("./demoDb.db", &db);
    if (rc) {
      _SPDLOG(logger_name, info, "cannot open db: {}", "demoDB.db");
      return http::HttpResponse(response_body, request);
    }
    char *zErrMsg = 0;
    std::string sql = "select * from blogs where blog like \"%" + query_string + "%\";";
    _SPDLOG(logger_name, info, "running sql query: {}", sql);
    std::string data = "<table border=\"1\"><tr><th>Date</th><th>Blog</th></tr>";
    rc = sqlite3_exec(db, sql.c_str(), sql_callback_select, (void *)&data, &zErrMsg);
    if( rc != SQLITE_OK ){
      _SPDLOG(logger_name, info, "select from db failed: {} with sql: {}", "demoDb.db", sql);
      _SPDLOG(logger_name, info, "sql {}: ", sql);
      _SPDLOG(logger_name, info, "error {}: ", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {

      data += "</table>";
      response_body += "<body>" + data + "<br>";
      _SPDLOG(logger_name, info, "result: {}", data);
    }
    sqlite3_close(db);
  } else {
    _SPDLOG(logger_name, info, "cannot find param: {}", "query");
  }



  std::string response_end_body = "<div><form action=\"/search_blog\" method=\"GET\"><input type=\"text\" name=\"search\" placeholder=\"query\"><input type=\"submit\" value=\"search\"></form></div>"
    "<div><form action=\"/insert_blog\" method=\"POST\"><textarea name=\"blog_post\" placeholder=\"write a blog...\"></textarea><input type=\"submit\" value=\"submit\"></form></div>"
    "</body></html>";
  response_body += response_end_body;

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

    return http::HttpResponse::render_to_response("../templates/home.html", request);
  } else if (request.get_method() == "POST"){
    _SPDLOG(logger_name, info, "home session id: {}", std::to_string(request.get_session_id()));
    auto session_map = request.get_session();
    auto params = request.get_parameters();
    auto first_name_result = params.find("firstname");
    std::string response_body = "<!DOCTYPE html><html><head><style> textarea {width: 100%;"
    "height: 150px;"
    "padding: 12px 20px;"
    "box-sizing: border-box;"
    "border: 2px solid #ccc;"
    "border-radius: 4px;"
    "background-color: #f8f8f8;"
    "font-size: 16px;"
    "resize: none;"
    "}"
    "</style>"
    "</head><body><p>welcome, ";
    if (first_name_result != params.end()) {
      response_body += first_name_result->second;
    }
    response_body += "<div><form action=\"/search_blog\" method=\"GET\"><input type=\"text\" name=\"search\" placeholder=\"query\"><input type=\"submit\" value=\"search\"></form></div>"
    "<div><form action=\"/insert_blog\" method=\"POST\"><textarea name=\"blog_post\" placeholder=\"write a blog...\"></textarea><input type=\"submit\" value=\"submit\"></form></div>"
    "</body></html>";
    if (first_name_result != params.end()) {
      _SPDLOG(logger_name, info, "found first name: {}", first_name_result->second);
      session_map->set("username", first_name_result->second);
    }
    /*
    sqlite3 *db;
    int rc = sqlite3_open("./demoDb.db", &db);
    if (rc) {
      _SPDLOG(logger_name, info, "cannot open db: {}", "demoDB.db");
      return http::HttpResponse(response_body, request);
    }


    std::string sql = "INSERT INTO users (username) VALUES (\"" + first_name_result->second + "\");";
    char *zErrMsg = 0;
    rc = sqlite3_exec(db, sql.c_str(), sql_callback_insert, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
      _SPDLOG(logger_name, info, "insert into db failed: {} with sql: {}", "demoDb.db", sql);
      _SPDLOG(logger_name, info, "sql {}: ", sql);
      _SPDLOG(logger_name, info, "error {}: ", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    sql = "select * from users;";

    std::string data;
    rc = sqlite3_exec(db, sql.c_str(), sql_callback_select, (void *)&data, &zErrMsg);
    if( rc != SQLITE_OK ){
      _SPDLOG(logger_name, info, "select from db failed: {} with sql: {}", "demoDb.db", sql);
      _SPDLOG(logger_name, info, "sql {}: ", sql);
      _SPDLOG(logger_name, info, "error {}: ", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      _SPDLOG(logger_name, info, "result: {}", data);
    }
    sqlite3_close(db);
    */
    auto resp = http::HttpResponse(response_body, request);

    _SPDLOG(logger_name, info, "{}", resp.to_string());
    return resp;
  } else {
    return http::HttpResponse("<html><p>Not Supported Request: " + request.get_method());
  }
}
