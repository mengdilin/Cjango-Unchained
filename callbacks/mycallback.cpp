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
#include "../mstch/include/mstch/mstch.hpp"

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
   std::vector<std::string> *pvect = static_cast<std::vector<std::string> *>(data);
   std::string row_data = "";
   for(i=0; i<argc; i++){

      row_data  += "<td>";
      row_data += argv[i];
      row_data += "</td>";
      //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   pvect->push_back(row_data);
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

int database_insert_blog(std::string blog, sqlite3 *db) {
  std::string now = datetime_now();
  std::string sql = "INSERT INTO blogs (date, blog) VALUES (\"" + now + "\", \"" + blog + "\");";
  char *zErrMsg = 0;
  int rc = sqlite3_exec(db, sql.c_str(), sql_callback_insert, 0, &zErrMsg);
  if( rc != SQLITE_OK ){
    _SPDLOG(logger_name, info, "insert into db failed: {} with sql: {}", "demoDb.db", sql);
    _SPDLOG(logger_name, info, "sql {}: ", sql);
    if (zErrMsg != 0) {
      _SPDLOG(logger_name, info, "error {}: ", zErrMsg);
      sqlite3_free(zErrMsg);
    }

    return -1;
  }
  return 0;
}

int database_open(sqlite3** db) {
  int rc = sqlite3_open("./demoDb.db", db);
  if (rc) {
    _SPDLOG(logger_name, info, "cannot open db: {}", "demoDB.db");
    return -1; //error
  }
  return 0;
}

std::vector<std::string> database_select_blog(std::string sql, sqlite3 *db) {
  std::vector<std::string> data1;

  char *zErrMsg = 0;
  std::vector<std::string> data;

  int rc = sqlite3_exec(db, sql.c_str(), sql_callback_select, (void *)&data, &zErrMsg);
  if( rc != SQLITE_OK ){
    _SPDLOG(logger_name, info, "select from db failed: {} with sql: {}", "demoDb.db", sql);
    _SPDLOG(logger_name, info, "sql {}: ", sql);
    _SPDLOG(logger_name, info, "error {}: ", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  return data;
}

std::string get_error_template(std::string error_msg) {
  std::ifstream error_s("../templates/error_template.html");
  mstch::map errorcontext {{"error_message", error_msg}};
  std::string errorview((std::istreambuf_iterator<char>(error_s)),
            std::istreambuf_iterator<char>());
  return mstch::render(errorview, errorcontext);
}

std::string get_success_template(std::string success_msg) {
  std::ifstream success_s("../templates/success_template.html");
  mstch::map successcontext {{"success_message", success_msg}};
  std::string successview((std::istreambuf_iterator<char>(success_s)),
            std::istreambuf_iterator<char>());
  return mstch::render(successview, successcontext);
}

extern "C" http::HttpResponse page_insert(http::HttpRequest request) {

  //turn off html tag escaping from mstch
  mstch::config::escape = [](const std::string& str) -> std::string {
    return str;
  };
  std::ifstream t("../templates/index.html");
  std::string view((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());

  mstch::array data_rows;
  mstch::map context;
  auto session_map = request.get_session();
  std::string username = session_map->get("username");
  if (username != "") {
    context.insert({"user", "Hello, " + username});
  }
  if (request.get_method() == "GET") {
    return http::HttpResponse(mstch::render(view, context), request);
  }

  auto wrote_blog = request.get_parameters().find("blog_post");
  std::string now = datetime_now();
  std::string blog;
  if (wrote_blog != request.get_parameters().end()) {
    blog = wrote_blog->second;
    _SPDLOG(logger_name, info, "found param: {}", blog);
    sqlite3 *db = nullptr;
    if (database_open(&db) == -1) {
      sqlite3_close(db);
      context.insert({"error", get_error_template("failed to open database")});
      return http::HttpResponse(mstch::render(view, context), request);
    }
    if (database_insert_blog(blog, db) == -1) {
      sqlite3_close(db);
      context.insert({"error", get_error_template("failed to insert blog to database")});
      return http::HttpResponse(mstch::render(view, context), request);
    }

    /*
    std::string sql = "select * from blogs;";
    std::vector<std::string> data = database_select_blog(sql, db);
    for (int i = 0; i < data.size(); i++) {
      data_rows.push_back( mstch::map{{"data", data.at(i)}});
    }
      context.insert({"data_rows", data_rows});
      sqlite3_close(db);
      return http::HttpResponse(mstch::render(view, context), request);
      */
    context.insert({"error", get_success_template("successfully inserted into database")});
    return http::HttpResponse(mstch::render(view, context), request);
    }
  return http::HttpResponse(mstch::render(view, context), request);
}


extern "C" http::HttpResponse page_index(http::HttpRequest request) {
  mstch::config::escape = [](const std::string& str) -> std::string {
  return str;
};
  std::ifstream t("../templates/index.html");
  std::string view((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());
  mstch::array data_rows;
  mstch::map context;

  _SPDLOG(logger_name, info, "index session id: {}", std::to_string(request.get_session_id()));

  auto session_map = request.get_session();
  std::string username = session_map->get("username");
  if (username != "") {
      context.insert({"user", "Hello, " + username});

  }

  auto query = request.get_parameters().find("search");
  std::string query_string;
  if (query != request.get_parameters().end()) {

    query_string = query->second;
    sqlite3 *db = nullptr;
    if (database_open(&db) == -1) {
      sqlite3_close(db);
      context.insert({"error", get_error_template("failed to open database")});
      return http::HttpResponse(mstch::render(view, context), request);
    }
    std::string sql = "select * from blogs where blog like \"%" + query_string + "%\";";
    std::vector<std::string> data = database_select_blog(sql, db);
    for (int i = 0; i < data.size(); i++) {
      data_rows.push_back( mstch::map{{"data", data.at(i)}});
    }
      context.insert({"data_rows", data_rows});

      sqlite3_close(db);
      return http::HttpResponse(mstch::render(view, context), request);
    }
  return http::HttpResponse(mstch::render(view, context), request);
}

extern "C" http::HttpResponse page_home(http::HttpRequest request) {

  if (request.get_method() == "GET") {
    auto session_map = request.get_session();
    return http::HttpResponse::render_to_response("../templates/home.html", request);
  } else if (request.get_method() == "POST"){
    _SPDLOG(logger_name, info, "home session id: {}", std::to_string(request.get_session_id()));
    auto session_map = request.get_session();
    auto params = request.get_parameters();
    auto first_name_result = params.find("firstname");
    if (first_name_result != params.end()) {
      session_map->set("username", first_name_result->second);
    }

    return page_index(request);
  }
}
