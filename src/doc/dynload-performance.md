# Dynamic Callback Loading

#### Consideration

At first, we thought to port Django's API as much as possible, and naively assumed that's a straightforward path. That was not the case. We soon found out there are a fairly large amount of design choices for implementing similar functionalities in different languages (for example, Django's `register()` function name is a reserved word for system registers in C++ and we chose the second popular name `add_route()`).

One of such big design issues is how to handle callback functions. **Callback functions** (callbacks) are the functions that handle coming HTTP requests and generate/return appropriate HTTP responses. Since writing new callbacks or updating existing callbacks are one common task in web developement, designing a confortable workflow for writing callbacks would cut a large portion of development stress and time.

In Python's Django, every source file updated at runtime can be reloadable, and updating callback functions are almost trivial.

In existing famous C++ web application frameworks, however, users have to recompile the entire application every time they change callbacks. All callbacks are defined in application routing logic, and cannot be loadable to a running app. We investigated three famous C++ frameworks (Crow, Silicon, and Treefrog) and confirmed all of the three have to recompile an app.

Cjango solves this issue by leveraging C's [Dynamic Loading](https://en.wikipedia.org/wiki/Dynamic_loading) functionality. In Cjango, **users can modify/add URL-callback hashmaps and callbacks without any server downtime**. All URL-to-callback mappings are written in `callbacks/urls.json`, which corresponds to `ursl.py` in Django. When you change the `urls.json` file, a special cjango thread which monitors the files detects the file change and dynamically reload user's new callbacks. This runtime configuration techinique is inspired by a 3D C++ racing game [HexRacer](http://elfery.net/projects/hexracer.html) which employs text configuration files as dynamic loading triggers. We will explain this Dynamic Callback Loading later with examples.


#### Example 1: Changing an existing callback to another function

When your main application is invoked, Cjango automatically spawns a file-monitoring thread by `spawn_monitor_thread()` before entering an http request handling event loop. The monotoring thread checks the `callbacks/urls.json` change for every one second by `App::monitor_file_change()`, and if it's changed, reloading the routing file to update callback hashmaps by `Router::load_url_pattern_from_file()`.

Note that if the specified `.so` file is not located to the path, Cjango instead loads a default callback function which returns `500 Internal Server Error` on web browsers and generates a debug message on a terminal.

Suppose you defined `render_with_db_fast` and `render_with_db_fast_v2` in your `callbacks/db-access.cpp`. If your callback function is written in a single file, you can compile your callback function  without writing one line of Make commands.

```bash
$ ls
db-access.cpp

$ make
g++ -std=c++1z -Wall -fPIC -c db-access.cpp
g++ -std=c++1z -Wall -shared -o db-access.so db-access.o -lhttp_response -lhttp_request

$ ls
db-access.cpp    db-access.o   db-access.so
```

Then, let's modify your `urls.json` from

```json
{
  "/booklist" : {
    "file" : "callbacks/db-access.so",
    "funcname": "render_with_db_fast"
  }
}

```

to

```json
{
  "/booklist" : {
    "file" : "callbacks/db-access.so",
    "funcname": "render_with_db_fast_v2"  # updated
  }
}
```

Right after you saved the `urls.json`, the aforementioned monitoring thread detects the change and automatically reloads your `render_with_db_fast_v2` callback function. You don't need a hassle to `make` an entire application -- just 2 characters change.

If you're a traditional C++ programer, you can also store old callback functions by shared object version numbers (e.g. `db-access.so.0.1` or `db-access.so.0.2`) and make a soft link to `db-access.so`.

##### URL Mapping files

System-wide URL mappings are written in `Cjango-Unchained/urls.json`, but users can also prepare per-app URL mappings. For example, the two example applications have each URL mapping file in the following path:       

+ `Cjango-Unchained/apps/http-get-demo/json/urls.json`  
+ `Cjango-Unchained/apps/http-post-demo/json/urls.json`

Where these `urls.json` are located is specified by `CALLBACKS` parameter in `setting.json`, which corresponds to `settings.py` in Django.

#### Example 2: Updating an existing callback to its newer version (with same name)

Since the file-monitoring thread just checks **the url-mapping file**, it's possible that the thread doesn't notice **the shared object file change** after you update your callback function. However, the solution is simple -- just to enable commented-out `touch urls.json` command. `touch` changes the `urls.json`'s recent modification time, and then Cjango can notice its change.

Internally, each object file is managed by reference counting. That means if users use the above `touch` trick, the file reference count has to be set as zero first and be increased later. Otherwise, only the old function can be referenced. This handling is performed in `Router::load_callback()`.


##### Error handling (invalid callbacks)

+ **Case 1: Typo in callback file/function name (non-existent callbacks)**. These cases are thrown as invalid function specified error in debug mode, and `500 Internal Server Error` in production mode.
+ **Case 2: No URL pattern match**. These cases are `404 Not Found` error on browser and warning message on console.

##### Dynamic Loading Internals: Casting

While Dynamic Loading is a powerful feature, it has to deal with any kind of types ranging from a function pointer of `double square_root(double)` to an object of `MyOriginalClass`. Therefore, it is natural that the type of returned symbol is by default a general pointer `void *`, and users need to cast the generic pointer to a corresponding type. We enclosed type-unsafe code region in just one function `Router::load_callback()`. This function returns a functor type `std::function<http::HttpResponse(http::HttpRequest)>` which is already casted within the function in order to maximize type safety.

# Performance Evaluation

### Experimental Settings

+ Execute Apache Bench command (`ab` Version 2.3), which is one of common HTTP benchmark tools.

| Server | Macbook Pro Mid 2014 | OS X 10.11.6 (El Capitan) | 3.0GHz 2 cores Core i7 | 16GB DDR3 Memory |
|:------:|:--------------------:|:-----------------------:|:----------------------:|:----------------:|
| Client |    Windows Laptop    |    Windows 7 (64bit)    | 2.4GHz 8 cores Core i7 |  8GB DDR3 Memory |
+ [Clt] -- 1Gbps CAT6 ethernet -- Router -- 1Gbps CAT6 Ether-Thunderbolt adapter -- [Srv]

### Experiments

Executed the following commands on the client machine.

+ `ab -n 10000 -c ${nc} -g ${name}.log http://192.168.0.11:8000/ > ${name}.txt`
  + This command sends HTTP requests to the specified IP address repeatedly.
  + `-n` is the total number of HTTP requests to send
  + The number of concurrency `${nc}` is set as 1, 3, 5, 7, 10, 50, 100, 150, 200, 250, ..., 500
    - Django cannot handle >10 concurrent requests (performance degraded significantly) and stopped there
    - all raw log files are stored under `Cjango-Unchained/src/bench/ether-ether/`

+ A simplest html file which just contains "\<html>HelloWorld\</html>" is served
  - all debug messages are disabled (cjango, django)
  - enabled optional multithread functionality (crow)
  - the number of URL pattern rules are fixed as 1 (all)

### Results

![](performance.png)

The x-axis is the number of concurrent HTTP requests (`nc`) and the y-axis is the average response time of **each bundle of** concurrent HTTP requests. In other words, if the target library scales well for large number of requests, the points will be roughly located on a straight line. The left zoomed graph shows **Cjango constantly outperforms our benchmark, Django.** As a conservative comparison, Cjango achieves more than **20% (= 11.2ms / 14.4ms) improvement for 10 concurrent requests**.

Moreover, the right graph shows that **our library can perform cómparably with other popular (Crow and Silicon are both 1000+ stared Github projects) C++ web app framework libraries** up to 500 concurrent requests. Overall, the fastest framework is [Silicon](https://github.com/matt-42/silicon) (high-speed Web API framework), which uses GNU's high-performant [libmicrohttpd](http://www.gnu.org/software/libmicrohttpd/) as backend.

These graphs support that **our design choices of original HTTP parser, original asynchronous request handler, and unique dynamic callback loader have zero or negligible performance losses**.

