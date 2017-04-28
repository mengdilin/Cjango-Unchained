
## Internal API

In general the functionalities are divided into four core modules:

* `App` ... handler of connections, data transfer and thread spawning
* `Router` ... handler URL path to function mapping and callback function loading
* `HttpRequest` ... Http request parser
* `HttpResponse` ... Http response generator

A typical workflow of handling incoming Http request is: upon request, `App` opens a new client socket and spawns a new worker thread, which calls `Router` to find the corresponding callback function that takes in an `HttpRequest` instance and returns an `HttpResponse` instance, `App` then sends the data back to the client sock.

### Class `App`

Main entry point to run Http server. It provides functions to start a single Http server, listen and accept client connections, handle requests by spawning working threads and transfer data via sockets.

The main public member functions are:

* `App(Router router)`

Constructor function to populate `App::router` data.

* `void run(int port)`

Starts Http server socket. Runs forever on the given port, setting up listening socket that polls for client connections. If there is a client connection, calls `handle_request(int client_socket)` to handle the request in a spawned thread. 

The socket operations are not using the popular `boost::asio` library so as to reduce dependencies and keep the library light-weighted. Since there is no socket functions in the C++ Standard Library, the traditional C-style socket is adopted. A wrapper class is provided to wrap the socket so that the resource can be deallocated in the destructor.

For asynchronous connections, sockets are set to be `O_NONBLOCK` and `select()` is used so that the main thread will not be blocked on `accept()`.

* `int App::handle_request(int client_socket)`

Given a client socket, it reads the raw Http request string from the socket and spawn a new thread by calling `std::thread` that copies the request and socket information for `App::worker()` to get the response.

* `void App::worker(int client_socket, string request)`

Worker thread function. Creates a HttpRequest object given the raw string, gets callback function by calling `Route::resolve(HttpRequest)` on the newly created Http request object, and sends back a Http response corresponding to the request back to the client socket. The socket is closed after `send()`.

The main public attributes are:

* `Router App::router`

A `Router` class instance that stores mapping from request paths to handler functions, used by `worker()` to get Http response.

### Class `Router`

Provides mapping from request paths to request handler functions, with dynamic loading feature.

The main public member functions are:

* `Router(map<string, functor> routes)`

Constructor that calls `Router::add_route()` to populate `Router::pattern_to_callback` and `Router::patterns_list` data.

* `Router::add_route(string url_pattern, functor callback)`

Callback must have the signature `HttpResponse f(HttpRequest)`. Given a `url_pattern` and a callback functor, we store this pair in `pattern_to_callback` member variable and update `patterns_list`. We throw an error if we are trying to register the same `url_pattern` with different callback functors. Note, `url_pattern` may be a regex pattern or an actual `url_path`.

* `string Router::resolve(HttpRequest)`

Given a Http request, it returns the `url_pattern` corresponding to the Http request. Example: a request for `"/abc/e"` will return `"/abc/e"`. 

Also allows regex resolution. Example: a `url_pattern` `"articles/([0-9]{4})/([0-9]{2})/$"` is registered, HttpRequest for url path `"articles/2014/12/"` shall return the `url_pattern`. Note that if there are 2 potential matching patterns for a given HttpRequest, we return the first matching pattern found in `patterns_list`.

* `HttpResponse Router::get_Http_response(HttpRequest)` 

Given a Http request, it returns the corresponding Http response.

The main public attributes are:

* `Router::pattern_to_callback`

A map that given a `url_path`, returns a callback function corresponding to the `url_pattern`.

* `Router::patterns_list` 

A vector of patterns, in the exact order of how the user registers the patterns.

Example:

```C++
       Router.add("/abc", some call back);
       Router.add("/efg", some call back);
       Router.add("/xyz", some call back);
       //patterns_list should have the patterns in the order: 
       //["/abc", "/efg", "/xyz"]       
```

### Class `HttpRequest*`
including:

`HttpRequest` `HttpRequestBodyParser` `HttpRequestLine` `HttpRequestParser`

Under namespace `"Http"`. Parses Http requests from string. Supports session using cookies.

The main public member functions are:

* `HttpRequest (std::string path)`
 `HttpRequest (std::string, std::string, std::string, std::unordered_map< std::string, std::string >, std::unordered_map< std::string, std::string >, std::unordered_map< std::string, std::string >)`

Constructor function.

* `std::string HttpRequest::get_method() const`
 
Getter for Http request's method. Ex: `GET`
 
* `std::string HttpRequest::get_path () const`
 	
Getter for Http request's path. Ex: `/abc.html` 
 
* `std::string HttpRequest::get_scheme () const`

Getter for Http request's scheme. Ex: `Http/1.0` 
 
* `unsigned long HttpRequest::get_session_id ()`

Getter for Http request's `session_id`, persisted via cookie mechanism. 
 
* `bool HttpRequest::has_session_id ()`
 
Check if Http request has a `session_id` associated with it.
 
* `std::unordered_map< std::string, std::string > const & 	HttpRequest::get_meta () const`

Getter for Http request's meta variables in the request header. 
 
* `std::unordered_map< std::string, std::string > const & 	HttpRequest::get_parameters () const`

Getter for Http request's GET/POST parameters.
 
* `std::unordered_map< std::string, std::string > const & 	HttpRequest::get_cookie () const`
 
Getter for Http request's cookie map.
 
* `std::shared_ptr< HttpSession > HttpRequest::get_session ()`

Getter for a session object pointer associated with the current Http request 

* `std::unordered_map< std::string, std::string > 	HttpRequestBodyParser::parse (std::istream &, std::string, int)`

Parses Http request body.

* `HttpRequest HttpRequestParser::parse_request_line_and_headers (std::istream &input_stream)`
 
Given an input_stream containing a http request, parses the request line and headers.
 
* `HttpRequest HttpRequestParser::parse (std::istream &input_stream)`

Given an input_stream containing a http request, parses the request line, headers, and body.

The main public atrributes are:

* `std::string HttpRequest::method`

Either `"GET"`, `"POST"`, `"HEAD"`. 
 
* `std::string HttpRequest::path`

A string representing the full path to the requested page, not including `"http"` or domain. Example: `"/music/bands/the_beatles/"`
 
* `std::string HttpRequest::scheme`

A string representing the scheme of the request (`http/version` or `https/version`). For simplicity sake, we only support `http/1.0`; thus, scheme is always set to `http/1.0`.
 
### Class `HttpResponse`

Under namespace `"http"`. Generates Http responses.

The main public member functions are:

* `HttpResponse (std::string)`
 `HttpResponse (std::string, HttpRequest &)`
 `HttpResponse (std::string, std::string)`
 `HttpResponse (int)`
 
Constructor functions.

* `std::string HttpResponse::to_string ()`
 
Returns a well-formated string version of http response compliant with the http protocol.
 
* `void HttpResponse::set_cookie (std::string, std::string)`

Given a key value pair, inserts the pair to request's cookie.
 
* `static HttpResponse HttpResponse::render_to_response (std::string)`

Given a file path, generate a http response.
 
* `static HttpResponse HttpResponse::render_to_response (std::string, std::string)`
 
Given a file path and content type of the file, generate a http response. 
 
* `static HttpResponse HttpResponse::render_to_response (std::string, HttpRequest &)`

Given a file path and a http request, generate a http response. 
 
* `static HttpResponse HttpResponse::render_to_response (std::string, std::string, HttpRequest &)`

Given a file path, content type of the file, and a http request, generate a http response.
 
* `static std::string HttpResponse::get_template (std::string path)`

Given a file path, generate a string containing file's data.

The main public attributes are:

* `std::string HttpResponse::content`
 
* `int HttpResponse::status_code`
 
* `std::string HttpResponse::reason_phrase`
 
* `std::string HttpResponse::http_version`
 
* `std::string HttpResponse::content_type`
 
* `std::unordered_map< std::string, std::string > HttpResponse::headers`
 
* `static std::unordered_map< int, std::string > HttpResponse::code_to_reason`
 
* `static std::string HttpResponse::templates_root`

#### Class `HttpSession`

Under namespace `"http"`. Session suppport.

* `std::string HttpSession::get(std::string)`
 	
Given a key in a session map, return its value. Returns an empty string if key not found.
 
* `void HttpSession::set(std::string, std::string)`
 	
Given a (key, value) pair, insert it to session map.

### Class `HttpStreamReader`

Under namespace `"http"`. Reads from standard stream.

* `std::string HttpStreamReader::get_next_line(std::istream &input_stream)`

Given an input stream, returns the current line.

* `std::string HttpStreamReader::to_string(std::istream &input_stream)`

Given an input stream, converts it to string.
 
* `std::string HttpStreamReader::read_util(std::istream &input_stream, int character)`
 
* `std::string HttpStreamReader::read(std::istream &input_stream, int length)`

Given an input stream and number of length bytes we want to read from the input stream, converts number of length bytes from input stream to a string.
 
* `void HttpStreamReader::eat_white_space(std::istream &input_stream)`

Removes white spaces from input stream.

### Class `UrlEncodedFormParser`

Under namespace `"http"`. 

The main public member functions are:

* `std::unordered_map< std::string, std::string > UrlEncodedFormParser ::get_parameter(std::istream &input_stream, int content_leng)`
 	
Given current input stream containing get or post params encoded as a single string, parses it into a map.
 
* `std::vector< std::string > UrlEncodedFormParser::split (std::string str, char delimiter)

Splits a string by a character delimiter and returns the result as a vector.

### Class `MSocket`

Simple wrapper class of POSIX socket and its operations.

### Class `Selector`

Simple wrapper class of `fd_set` used for `select()` function.