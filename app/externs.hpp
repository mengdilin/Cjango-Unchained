#ifndef _EXTERNS_HPP
#define _EXTERNS_HPP

#include <iostream>
#include <map>
#include <functional>
#include <string>
#include "../routing/router.hpp"
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
    std::cout << std::endl;\
} while(0)
#else
#define _DEBUG(...) do{} while(0)
#endif

inline HttpResponse get_phony_response(HttpRequest req)
{
    string text = "<html>\r\n<body>\r\n"
                 "<h1>Hi there!</h1>\r\n"
                 "<p>This is just a phony response :P</p>\r\n"
                 "</body>\r\n</html>";
    HttpResponse resp(text);
    return resp;
}

#ifdef DYNLOAD_CJANGO
extern bool global_is_file_updated;
// If you see "duplicated symbol" error when linking (not when compiling),
// that's because you put definitions in your .hpp file... like me
void *load_shared_object_file(const std::string& path);
void *load_callback(void *lib, const std::string& func_name);
#endif

#endif
