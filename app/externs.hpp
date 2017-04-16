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

#include <spdlog/spdlog.h>
#include <unordered_map>
// CjangoLogger is a wrapper class of spdlog::logger
// This wrapper class gives spdlog the power to define a new logger if undefined
// Then users don't have to initialize a new logger -- just use it
// This is the functionality not in spdlog I believe
class CjangoLogger {
    std::unordered_map<std::string, std::shared_ptr<spdlog::logger>> loggers;
public:
    CjangoLogger(){};
    std::shared_ptr<spdlog::logger> operator[](std::string name) {
        auto found = loggers.find(name) != loggers.end();
        if (! found) {
            // add a new logger
            std::shared_ptr<spdlog::logger> l = spdlog::stdout_color_mt(name);
            loggers[name] = l;
        }
        return loggers[name];
    };
};
extern CjangoLogger cjango_loggers;


// https://github.com/gabime/spdlog/issues/340#issuecomment-287583891
#define _SPDLOG(logger_name, level, ...) (cjango_loggers)[logger_name]->level("[" + fmt::format(__FILE__) + ":" + std::to_string(__LINE__) + ":" + string(__FUNCTION__) + "] " + fmt::format(__VA_ARGS__))

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
    std::cout << ":" << __FUNCTION__;\
    std::cout << ":" << __LINE__ << " ] ";\
    _log(__VA_ARGS__);\
    std::cout << std::endl;\
} while(0)

#else
#define _DEBUG(...) do{} while(0)
#define _SPDLOG(...) do{} while(0)
#endif

inline http::HttpResponse get_phony_response(http::HttpRequest req)
{
    string text = "<html>\r\n<body>\r\n"
                 "<h1>Hi there!</h1>\r\n"
                 "<p>This is just a phony response :P</p>\r\n"
                 "</body>\r\n</html>";
    http::HttpResponse resp(text);
    return resp;
}

#ifdef CJANGO_DYNLOAD
// If you see "duplicated symbol" error when linking (not when compiling),
// that's because you put definitions in your .hpp file... like me
void *load_shared_object_file(const std::string& path);
void *load_callback(void *lib, const std::string& func_name);
#endif

#endif
