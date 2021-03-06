#ifndef _APP_HPP
#define _APP_HPP

#include <string>
#include <cstring>
#include "externs.hpp"

#ifdef CJANGO_DYNLOAD
#include <regex>
#include <FileWatcher.h>
#endif

using namespace std;

#ifdef CJANGO_DYNLOAD
/// Processes a file action
class UpdateListener : public FW::FileWatchListener
{
public:
    UpdateListener() {}
    void handleFileAction(
        FW::WatchID watchid,
        const FW::String& dir,
        const FW::String& filename,
        FW::Action action, bool& is_updated
    ) {
        // In order to share a update flag between monitor process and main process
        // without using a global variable, I needed to add is_update argument.
        // However, adding is_updated is actually a LARGE change because:
        //      1. The current library (SimpleFileWatcher) is cross-platform
        //      2. It's implemented by abstract class and needed to rewrite
        //         arguments of all relative functions
        //
        // Here I only handled file update calse correctly (because that's the only use case)
        // and left all dummy booleans (is_updated_dummy) under /lib/simplefilewatcher.
        // We might consider other libraries if time permitted.


        // Currently only a single file but might be expanded to other files.
        // Thus using regex instead of fixed pattern matching
        std::regex urls_rgx("urls.json");
        std::smatch match;

        if (std::regex_search(filename.begin(), filename.end(), match, urls_rgx)) {
            _SPDLOG("update", info, "{}:{}:{}", dir, filename, action);
            is_updated = true;
        }

        std::regex settings_rgx("settings.json");
        if (std::regex_search(filename.begin(), filename.end(), match, settings_rgx)) {
            _SPDLOG("update", info, "{}:{}:{}", dir, filename, action);
            is_updated = true; // FIXME return filenames
        }
    }
};
#endif

class App {
    int servSock; /* server socket id */
public:
#ifndef CJANGO_DYNLOAD
    App() : servSock{-1} {
#else
    App() : servSock{-1}, is_file_updated(false) {
#endif
    }

    Router router;

#ifdef CJANGO_DYNLOAD
    UpdateListener listener;
    FW::FileWatcher fileWatcher;
    bool is_file_updated;
    void monitor_file_change();
    void spawn_monitor_thread();
    std::string urls_json_dir;
#endif

    /**
    ** @brief add a <url, function> mapping to the Router instance
    */
    void add_route(std::string url_pattern, functor f) {
      router.add_route(url_pattern, f);
    }
    void worker(int clntSock, string strRequest);

    void reload_url_mappings() { router.load_url_pattern_from_file(urls_json_dir); };
    void add_monitored_dir(const std::string dir);
    void set_urls_json_dir(std::string dir) { urls_json_dir = dir; };
    std::string get_urls_json_dir() const { return urls_json_dir; };
    
    void run(int port);
    int handle_request(int socket);
};
#endif

