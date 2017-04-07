#ifndef _APP_HPP
#define _APP_HPP

#include <string>
#include <cstring>
#include "externs.hpp"

#ifdef DYNLOAD_CJANGO
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
        FW::Action action)
    {
        _DEBUG(dir, " -> ", filename, " has event ", action, "\n\n\n");
        global_is_file_updated = true;
    }
};
#endif

class App {
public:
    Router router; // FIXME how router can be private?
#ifdef DYNLOAD_CJANGO
    UpdateListener listener;
    FW::FileWatcher fileWatcher;
#endif
    void add_route(std::string url_pattern, functor f) {
      router.add_route(url_pattern, f);
    }
    App() {
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
