# Cjango-Unchained

<img align="center" src="src/doc/logo.png">

Cjango is a lightweight C++ Web framework that provides high-speed server responses and aims for [Django](https://github.com/django/django)-like usability.

<!-- ![](performance.png) -->

## Installation

```bash
git clone git@github.com:mengdilin/Cjango-Unchained.git

# -std=c++1z needs recent g++, so
# on Ubuntu 14.04
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get install g++-6

cd Cjango-Unchained

make
# or make -j[number of cores] on multicore machines
# e.g. "make -j2"

# executing Makefile on callbacks/ subdir or "cd callbacks && make"
make -C callbacks
```

## Usage

```bash
python manage.py runserver 8000         # if you prefer more Django-like execution
```

or

```
./runapp runserver 8000                 # directly execute C++ if you don't have Python installed
```

And access to `http://127.0.0.1:8000` shows Cjango welcome page.

## Main files

- `Cjango-Unchained` ... root directory (can be renamed)
  + `callbacks` ... default directory for you to write C++ functions
  + `json` ... json files for app configurations
  + `templates` ... default directory for storing template html files
  + `static` ... default directory for static files like images
  + `src` ... Cjango C++ codes. Usually you don't have to look into it.
  + `Makefile` ... Makefile to compile Cjango
  + `manage.py` ... A wrapper script for running `src/runapp` executable conveniently


## Features

### Asyncronous request handling

<!-- Cjango handles client requests asyncronously by `select` system calls. Compared to Django, Our benchmark shows 3-5x performance benefits on basic http request handling. Cjango has comparably fast performance with famous C++ web framework projects such as [Crow](https://github.com/ipkn/crow) or [Silicon](https://github.com/matt-42/silicon). -->

### Regular expression URL matching

As in Django, Cjango deal with URL routing by regex matching. All matching rules are written in `urls.json` as like in Django's `urls.py`. The earlier rules have higher priorities (first-match, first-served).

`urls.json` is dynamically (i.e. at runtime) loaded into our routing logic and enables **each callback function to be compiled separately from main application**. This is the most notable functionality in Cjango.

### Template file rendering by `HttpResponse::render_to_response()`

All template files are placed under `callbacks/templates`. And in source files, they can be referenced by `callbacks/templates/<file name>` (Note: the relative path is started from `runapp` executable, not from each callback object files).

### HTTP 1.0 Compatibility with Session Support

Cjango provides a HttpSession object that provides a way to identify a user across more than one page request or visit to a Web site and to store information about that user. The session object is implemented on top of the cookie functionality that Cjango also provides.

As of April 2017, Cjango can handle HTTP 1.0 requests and responses.

Cjango supports Http session (similar with `django.contrib.sessions`).

### Json Support

```bash
python manage.py runserver 8000
```

```bash
# On another terminal

PCUser@abc callbacks (master) $ curl -s http://127.0.0.1:8000/json | jq '.'
{
  "parents": [
    {
      "sha": "54b9c9bdb225af5d886466d72f47eafc51acb4f7",
      "url": "https://api.github.com/repos/stedolan/jq/commits/54b",
      "html_url": "https://github.com/stedolan/jq/commit/54b"
    },
    {
      "sha": "8b1b503609c161fea4b003a7179b3fbb2dd4345a",
      "url": "https://api.github.com/repos/stedolan/jq/commits/8b1",
      "html_url": "https://github.com/stedolan/jq/commit/8b1"
    }
  ]
}
PCUser@abc callbacks (master) $ curl -s http://127.0.0.1:8000/json | jq '.parents[0]'
{
  "sha": "54b9c9bdb225af5d886466d72f47eafc51acb4f7",
  "url": "https://api.github.com/repos/stedolan/jq/commits/54b",
  "html_url": "https://github.com/stedolan/jq/commit/54b"
}
```

### Whitelist-based debug messages

Suppose that you compiled Cjango in debug mode (`make DEBUG=1`). If you run Cjango simply by `./runapp runserver 8000` , you would see a flooding number of debugging messages like this:

```
user@host Cjango-Unchained $ ./runapp runserver 8000

[20170416 20:00:03.857] [route] [info] [router.cpp:112:load_all] loaded urls.json
[20170416 20:00:03.857] [route] [info] [router.cpp:19:add_route] updated route: /
[20170416 20:00:03.858] [route] [info] [router.cpp:19:add_route] updated route: /cjango
[20170416 20:00:03.858] [route] [info] [router.cpp:19:add_route] updated route: /[0-9]{4}/[0-9]{2}
[20170416 20:00:03.859] [route] [info] [router.cpp:19:add_route] updated route: /home
[20170416 20:00:03.860] [skt] [info] [app.cpp:292:run] Invoked for port: 8000
[20170416 20:00:03.860] [skt] [info] [app.cpp:304:run] Created server socket: 5
[20170416 20:00:03.860] [skt] [info] [app.cpp:250:spawn_monitor_thread] detached a new thread
[20170416 20:00:11.428] [skt] [info] [app.cpp:376:run] Number of sockets readable: 1
[20170416 20:00:11.428] [skt] [info] [app.cpp:385:run] Server socket readable
[20170416 20:00:11.582] [skt] [info] [app.cpp:403:run] Client socket 21 readable
[20170416 20:00:11.582] [http] [info] [req_parser.cpp:134:get_http_request_line] uri fields:
[20170416 20:00:11.583] [http] [info] [req_parser.cpp:71:get_http_cookie] cookie: csrftoken=...
[20170416 20:00:11.583] [http] [info] [req_parser.cpp:71:get_http_cookie] cookie: session=10...
[20170416 20:00:11.583] [skt] [info] [app.cpp:119:worker] finished request
[20170416 20:00:11.583] [route] [info] [router.cpp:159:get_http_response] ret callback for /home
[20170416 20:00:11.583] [http] [info] [http_req.cpp:90:get_session] cookie:   session, 10596601
[20170416 20:00:11.583] [http] [info] [http_req.cpp:95:get_session] found session key: 10596601
[20170416 20:00:11.583] [http] [info] [http_req.cpp:96:get_session] key equals 604414511: false
[20170416 20:00:11.583] [http] [info] [http_req.cpp:102:get_session] cannot find session id: 1059...
[20170416 20:00:11.583] [skt] [info] [app.cpp:216:handle_request] Created and detached new thread
[20170416 20:00:11.584] [session] [info] [mycall.cpp:39:page_index] session id: 10596601668567
[20170416 20:00:11.584] [session] [info] [mycall.cpp:41:page_index] session:  user, unspecified
[20170416 20:00:11.584] [skt] [info] [app.cpp:155:worker] Worker thread closing socket 21
[20170416 20:00:11.902] [skt] [info] [app.cpp:376:run] Number of sockets readable: 1
[20170416 20:00:11.902] [skt] [info] [app.cpp:403:run] Client socket 22 readable
[20170416 20:00:11.902] [skt] [info] [app.cpp:174:handle_request] socket 22 recv returns: 677
[20170416 20:00:11.902] [skt] [info] [app.cpp:174:handle_request] socket 22 recv returns: -1
[20170416 20:00:11.902] [skt] [info] [app.cpp:201:handle_request] May try again later on socket 22
[20170416 20:00:11.902] [skt] [info] [app.cpp:107:worker] Worker thread invoked for socket 22
...
```

But if you're debugging your original callbacks with http session fucntionality, and if you know other modules are working correctly, you would like to suppress irrelavant log messages.

Cjango can hide non-informative log messages by runtime `--whitelist` argument.

```
./runapp runserver 8080 --whitelist session http

[2017-04-16 20:00:11.582] [http] [info] [req_parser.cpp:134:get_req_line] uri fields:
[2017-04-16 20:00:11.583] [http] [info] [req_parser.cpp:71:get_cookie] cookie pair:  csrftoken=TN
[2017-04-16 20:00:11.583] [http] [info] [req_parser.cpp:71:get_cookie] cookie pair:  session=1059
[2017-04-16 20:00:11.583] [http] [info] [req.cpp:90:get_session] cookie:  session, 10596601668567
[2017-04-16 20:00:11.583] [http] [info] [req.cpp:95:get_session] found session key: 10596601668567
[2017-04-16 20:00:11.583] [http] [info] [req.cpp:96:get_session] key equals 60441451194812: false
[2017-04-16 20:00:11.583] [http] [info] [req.cpp:102:get_session] cannot find session id: 1059
[2017-04-16 20:00:11.584] [session] [info] [mycallback.cpp:39:page_index] index session id: 10596601668567
[2017-04-16 20:00:11.584] [session] [info] [mycallback.cpp:41:page_index] session:  user, unspecified
```

#### User-defined logger category

Moreover, you can even add your own logger category in your callback functions *without recompiling your main app executable*:

```
extern "C" http::HttpResponse render_top_page(http::HttpRequest request) {
      ...
      _SPDLOG("cookie-v2", info, "state:  {}, {}", it->first, it->second);
      ...
}
```

```
[2017-04-16 20:00:11.584] [cookie-v2] [info] [your_callback.cpp:39:render_top_page] state: 32 58
```

#### How does this work?

Cjango defines a convenient `CjangoLogger` class on top of [Spdlog C++ logger library](https://github.com/gabime/spdlog/).

More specifically, `CjangoLogger::operator[]` checks whether the specified-name logger exists before logging. If no corresponding logger exists, it would spawn a new logger with the specified name. Thus, you can add your own logger category at runtime. Again, your callback function can be compiled separately from main Cjango application*. This approach is the opposite approach of `std::vector::operator[]`, which does not checks out-of-range condition in priority to performance.

#### Suppressing logs

While the above flexible log handling incurs slight runtime overhead, `make DEBUG=0` disables all debugging messages and set your application in production mode. In production mode, all debugging functions are removed by preprocessing and incur no runtime overhead.

### Dynamic Callback Loading


#### Consideration

At first, we thought to port Django's API as much as possible, and naively assumed that's a straightforward path. That was not the case. We soon found out there are a fairly large amount of design choices for implementing similar functionalities in different languages. One such example is callback handlings. Callback functions are the functions that handle coming HTTP requests and return appropriate HTTP responses. In Python's Django, every source file updated at runtime can be reloadable, and updating callback functitons are straightforward.

In existing famous C++ web application frameworks, users have to recompile the entire application every time they change callback functions which handle coming HTTP requests. All callback functions are defined in application routing logic, and cannot be loadable to a running app as far as we researched.

Cjango solves this issue by leveraging [Dynamic Loading](https://en.wikipedia.org/wiki/Dynamic_loading) functionality. In Cjango, users can modify/add URL-callback hashmaps and callbacks themselves **without any server downtime**. All URL-callback mappings are written in `callbacks/urls.json`. When you change the `urls.json` file, cjango monitors and detects the json file change and dynamically reload your new functions. This is inspired by a 3D C++ racing game "[HexRacer](http://elfery.net/projects/hexracer.html)" which employs text configuration files as dynamic loading triggers.

##### Example 1: Changing an existing callback to another function

When your main application is invoked, Cjango automatically spawns a file-monitoring thread. The monotoring thread checks the `urls.json` change for every one second, and if it's changed, reloading the routing file to update callback hashmaps.

For example, suppose you defined `render_with_db_fast` and `render_with_db_fast_v2` in your `callbacks/db-access.cpp`. If your callback function is written in a single file, you can compile your callback function  without writing single line of Make commands.

```
$ make
g++ -std=c++1z -Wall -DCJANGO_DYNLOAD -I./../app/ -I./../lib/ -fPIC -c db-access.cpp
g++ -std=c++1z -Wall -DCJANGO_DYNLOAD -I./../app/ -I./../lib/ -L./../app/ -lhttp_response -lhttp_request -shared -o db-access.so db-access.o

$ ls
db-access.cpp    db-access.o   db-access.so   urls.json
```

Then, let's modify your `urls.json` from

```
{
  "/booklist" : {
    "file" : "callbacks/db-access.so",
    "funcname": "render_with_db_fast"
  }
}

```

to

```
{
  "/booklist" : {
    "file" : "callbacks/db-access.so",
    "funcname": "render_with_db_fast_v2"
  }
}
```

Immediately after you saved the `urls.json`, Cjango's file-monitoring thread detects the change and automatically reloads your `render_with_db_fast_v2` callback function. You don't need a hassle to `make` all application -- just 2 characters change.

If you're a traditional C++ programer, you can also stores old callback functions by shared object version numbers (e.g. `db-access.so.0.1` or `db-access.so.0.2`) and a soft link to `db-access.so`.


##### Example 2: Updating an existing callback to its newer version (with same name)

Since the file-monitoring thread just checks **the url-mapping file**, it's possible that the thread doesn't notice **the shared object file** even if you updated your callback function. However, the solution is simple -- just to enable commented-out `touch urls.json` command. `touch` changes the `urls.json`'s recent modification time, and then Cjango can notice its change.


##### Error Handlings

+ **Case 1: Typo in callback file/function name (non-existent callbacks)**. These cases are thrown as invalid function specified error in debug mode, and `500 Internal Server Error` in production mode.
+ **Case 2: No URL pattern match**. These cases are `404 Not Found` error.

### Credits

#### Libraries
+ Json parser library: [nlohmann/json](https://github.com/nlohmann/json) (MIT)
+ File monitoring library: [simplefilewatcher](https://github.com/apetrone/simplefilewatcher) (MIT)
+ Fast logging library: [spdlog](https://github.com/gabime/spdlog/) (MIT)