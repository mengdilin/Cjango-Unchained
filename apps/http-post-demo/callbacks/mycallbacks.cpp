#include <cjango>
#include <unordered_map>
#include <sqlite3.h>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <string>
#include <fstream>
#include <streambuf>
#include <vector>

std::string logger_name = "session"; // use both in page_index() and page_home()

inline int sql_callback_insert(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

inline int sql_callback_select(void *data, int argc, char **argv, char **azColName){
   int i;
   std::string *pvect = static_cast<std::string *>(data);
   *(pvect) += "<tr>";
   for(i=0; i<argc; i++){

      *(pvect)  += "<td>";
      *(pvect) += argv[i];
      *(pvect) += "</td>";
   }
   *(pvect) += "</tr>";
   return 0;
}

inline std::string datetime_now() {
  auto now = std::chrono::system_clock::now();
  std::time_t now_clock = std::chrono::system_clock::to_time_t(now);
  std::stringstream ss;
  ss << std::put_time(std::localtime(&now_clock), "%c");
  return ss.str();
}

int database_insert_blog(std::string blog, sqlite3 *db) {
  std::string now = datetime_now();
  std::string sql = "INSERT INTO blogs (date, blog) VALUES (\"" + now + "\", \"" + blog + "\");";
  char *zErrMsg = 0;
  int rc = sqlite3_exec(db, sql.c_str(), sql_callback_insert, 0, &zErrMsg);
  if( rc != SQLITE_OK ){
    if (zErrMsg != 0) {
      sqlite3_free(zErrMsg);
    }

    return -1;
  }
  return 0;
}

int database_open(sqlite3** db) {
  int rc = sqlite3_open("./demoDb.db", db);
  if (rc) {
    return -1; //error
  }
  return 0;
}

std::string database_select_blog(std::string sql, sqlite3 *db) {
  char *zErrMsg = 0;
  std::string data;

  int rc = sqlite3_exec(db, sql.c_str(), sql_callback_select, (void *)&data, &zErrMsg);
  if( rc != SQLITE_OK ){
    sqlite3_free(zErrMsg);
  }
  return data;
}

std::string get_error_template(std::string error_msg) {

  std::string errorview = http::HttpResponse::get_template("error_template.html");
   replace(errorview, "{{error_message}}", error_msg);
   return errorview;
}

std::string get_success_template(std::string success_msg) {

  std::string successview = http::HttpResponse::get_template("success_template.html");
  replace(successview, "{{success_message}}",success_msg);
  return successview;
}

extern "C" http::HttpResponse page_insert(http::HttpRequest request) {
  std::string view = http::HttpResponse::get_template("index.html");
  replace(view, "{{data}}","");
  auto session_map = request.get_session();
  std::string username = session_map->get("username");
  if (username != "") {
    replace(view, "{{user}}", "Hello, "+username);
  } else {
    replace(view, "{{user}}","");
  }
  if (request.get_method() == "GET") {
    replace(view, "{{error}}","");
    return http::HttpResponse(view, request);
  }

  auto wrote_blog = request.get_parameters().find("blog_post");
  std::string now = datetime_now();
  std::string blog;
  if (wrote_blog != request.get_parameters().end()) {
    blog = wrote_blog->second;
    //_SPDLOG(logger_name, info, "found param: {}", blog);
    sqlite3 *db = nullptr;
    if (database_open(&db) == -1) {
      sqlite3_close(db);
      replace(view, "{{error}}", get_error_template("failed to open database"));
      return http::HttpResponse(view, request);
    }
    if (database_insert_blog(blog, db) == -1) {
      sqlite3_close(db);
      replace(view, "{{error}}", get_error_template("failed to insert blog to database"));
      return http::HttpResponse(view, request);
    }
    replace(view, "{{error}}", get_success_template("successfully inserted into database"));
    return http::HttpResponse(view, request);
    }
    replace(view, "{{error}}", get_success_template("successfully inserted into database"));
    return http::HttpResponse(view, request);
}


extern "C" http::HttpResponse page_index(http::HttpRequest request) {
  std::string view = http::HttpResponse::get_template("index.html");
  auto session_map = request.get_session();
  std::string username = session_map->get("username");
  if (username != "") {
      replace(view, "{{user}}", "Hello, "+username);

  } else {
    replace(view, "{{user}}","");
  }
  auto query = request.get_parameters().find("search");
  std::string query_string;
  if (query != request.get_parameters().end()) {

    query_string = query->second;
    sqlite3 *db = nullptr;
    if (database_open(&db) == -1) {
      sqlite3_close(db);
      replace(view, "{{error}}", get_error_template("failed to open database"));
      return http::HttpResponse(view, request);
    }
    std::string sql = "select * from blogs where blog like \"%" + query_string + "%\";";
    std::string data = database_select_blog(sql, db);
    replace(view, "{{data}}", data);
      sqlite3_close(db);
      replace(view, "{{error}}","");
      return http::HttpResponse(view, request);
    }
return http::HttpResponse(view, request);
}

extern "C" http::HttpResponse page_home(http::HttpRequest request) {

  if (request.get_method() == "GET") {
    auto session_map = request.get_session();
    return http::HttpResponse::render_to_response("home.html", request);
  } else if (request.get_method() == "POST"){
    auto session_map = request.get_session();
    auto params = request.get_parameters();
    std::string username ="";
    auto first_name_result = params.find("firstname");
    if (first_name_result != params.end()) {
      username = first_name_result->second;
      session_map->set("username", first_name_result->second);
    }
    std::string view = http::HttpResponse::get_template("index.html");
    replace(view, "{{user}}", username);
    replace(view, "{{error}}","");
    replace(view, "{{data}}","");
    return http::HttpResponse(view, request);
  }
}
