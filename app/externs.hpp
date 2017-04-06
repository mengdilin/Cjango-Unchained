#ifndef _EXTERNS_HPP
#define _EXTERNS_HPP

#include <iostream>
#include <map>
#include <functional>
#include <string>

using std::string;

/* print debug messages if -DDEBUG specified */
#ifdef DEBUG
inline void _log() {}
template<typename Head, typename ...Rest>
inline void _log(Head && h, Rest && ...r)
{
    std::cout << std::forward<Head>(h);
    _log(std::forward<Rest>(r)...);
}
#define _DEBUG(...) do\
{\
    std::cout << "[ " << __FILE__;\
    std::cout << " -> " << __FUNCTION__;\
    std::cout << " -> " << __LINE__ << " ] ";\
    _log(__VA_ARGS__);\
    std::cout << endl;\
} while(0)
#else
#define _DEBUG(...) do{} while(0)
#endif

class HttpRequest {
public:
    string data;
    HttpRequest(string str) : data(str) {}
};

class HttpResponse {
public:
    string content;
    HttpResponse(string str) : content(str) {}
};

inline HttpResponse get_phony_response(HttpRequest req)
{
    string text = "<html>\r\n<body>\r\n"
                 "<h1>Hi there!</h1>\r\n"
                 "<p>This is just a phony response :P</p>\r\n"
                 "<p>" + req.data + "</p>\r\n"
                 "</body>\r\n</html>";
    HttpResponse resp(text);
    return resp;
}

typedef std::function<HttpResponse(HttpRequest)> functor;
typedef std::map<std::string, functor> URLmap;

class Router {
public:
    URLmap pattern_to_callback;
    Router(URLmap routes): pattern_to_callback(routes) {}
    Router() {};
    HttpResponse get_http_response(HttpRequest req) {
        return get_phony_response(req);
    }
};

#endif