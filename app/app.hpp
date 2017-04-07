#ifndef _APP_HPP
#define _APP_HPP

#include <string>
#include <cstring>
#include "externs.hpp"

#ifdef DYNLOAD_CJANGO
#include <regex>
#include <FileWatcher.h>
#endif

using namespace std;

#ifdef DYNLOAD_CJANGO
/// Processes a file action
class UpdateListener : public FW::FileWatchListener
{
public:
    UpdateListener() {}
    void handleFileAction(FW::WatchID watchid, const FW::String& dir, const FW::String& filename,
        FW::Action action, bool& is_updated)
    {
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
        std::regex rgx("url-pattern.json");
        std::smatch match;

        if (std::regex_search(filename.begin(), filename.end(), match, rgx)) {
            _DEBUG(dir, " -> ", filename, " has event ", action, "\n\n\n");
            is_updated = true;
        }
    }
};
#endif

class App {
public:
    Router router; // FIXME how router can be private?
#ifdef DYNLOAD_CJANGO
    UpdateListener listener;
    FW::FileWatcher fileWatcher;
    bool is_file_updated;
#endif
    void add_route(std::string url_pattern, functor f) {
      router.add_route(url_pattern, f);
    }
    void monitor_file_change();
    App() : is_file_updated(false) {
#ifdef DYNLOAD_CJANGO
        router.load_url_pattern_from_file();
        // create the listener (before the file watcher - so it gets destroyed after the file watcher)

        // add a watch to the system
        // the file watcher doesn't manage the pointer to the listener - so make sure you don't just
        // allocate a listener here and expect the file watcher to manage it - there will be a leak!
        // FW::WatchID watchID =
        fileWatcher.addWatch("./callbacks", &listener, true);
#endif
    }
    // App(Router& rt): router(rt) {}
    void worker(int clntSock);
    void print_routes();
    void run(int port);
    void handle_request(int socket);
};

#endif
