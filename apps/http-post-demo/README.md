## Dependencies
This application uses sqlite3 as its backend database. To install sqlite3,

```bash
sudo apt-get install libsqlite3-dev
```
Alternatively, for your convenience, a snapshot of sqlite3 is included in this directory. To build and install,

```bash
cd sqlite-snapshot-201704181120/
./configure --prefix=/usr/local
make
sudo make install
```

## To Run
```bash
cd ../../
make -C demo/http-post-demo/callbacks
python manage.py runserver 8000 --setting apps/http-post-demo/json/settings.json

```


## Writing your second Cjango Application

Now that you understand how Cjango works, we can start writing a slightly more complicated web server. Web forms are one of the most basic building blocks in any web application. We will be using forms to allow users to log in to the application.

# Configuration
All configurations are specified in `json/settings.json` which specifies paths to `templates`, `callbacks`, `static`, and `json` directory.

## Step 1: Home page callback

Let's write a callback function that returns a html page when receiving a `GET` request.

```c++
extern "C" http::HttpResponse page_home(http::HttpRequest request) {

  if (request.get_method() == "GET") {
    auto session_map = request.get_session();
    return http::HttpResponse::render_to_response("home.html", request);
  }
}

```

This will return `apps/http-post-demo/templates/home.html` which is a page with a login form that prompts the user for a username.

## Step 2: Distinguishing POST from GET
Now, we need to handle `POST` requests which will be sent after the user has entered his username and clicked submit. The new `page_home` looks like:

```c++
extern "C" http::HttpResponse page_home(http::HttpRequest request) {

  if (request.get_method() == "GET") {
    auto session_map = request.get_session();
    return http::HttpResponse::render_to_response("home.html", request);
  } else if (request.get_method() == "POST"){
    std::string view = http::HttpResponse::get_template("index.html");
    return http::HttpResponse(view, request);
  }
}
```
New `page_home` now returns a different page when it receives a `POST` request.

## Step 3: Retrieve parameters from http requests
But how do we get the username the user has entered in `home.html`? You can access form parameters using `HttpRequest`'s `get_parameters()` which returns a `unordered_map<string, string>` containing all parameters of a http request.

```c++
extern "C" http::HttpResponse page_home(http::HttpRequest request) {

  if (request.get_method() == "GET") {
    auto session_map = request.get_session();
    return http::HttpResponse::render_to_response("home.html", request);
  } else if (request.get_method() == "POST"){
    auto params = request.get_parameters();
    std::string username ="";
    auto first_name_result = params.find("firstname");
    if (first_name_result != params.end()) {
      username = first_name_result->second;
    }
    std::string view = http::HttpResponse::get_template("index.html");
    replace(view, "{{user}}", username);
    return http::HttpResponse(view, request);
  }
}
```
New `page_home` retrieves the username entered by the user and inserts it into `index.html`.

## Step 4: HttpSession
But what if we want to remember the username of the same user across different requests? `HttpRequest` also provides a useful `HttpSession` object which provides a way to identify a user across more than one page request or visit to a Web site and to store information about that user. Let's put the username into a `HttpSession` object.

```c++
extern "C" http::HttpResponse page_home(http::HttpRequest request) {

  if (request.get_method() == "GET") {
    auto session_map = request.get_session();
    return http::HttpResponse::render_to_response("home.html", request);
  } else if (request.get_method() == "POST"){
    auto session_map = request.get_session();
    auto params = request.get_parameters();
    std::string username ="";
    auto first_name_result = params.find("firstname");
    if (first_name_result != params.end()) {
      username = first_name_result->second;
      session_map->set("username", first_name_result->second);
    }
    std::string view = http::HttpResponse::get_template("index.html");
    replace(view, "{{user}}", username);
    return http::HttpResponse(view, request);
  }
}
```
Now, username will be stored under the key `username`. If the user visits the website with the same browser client, we will always be able to retrieve his username by using `HttpSession`. Under the hood, `HttpSession` is implemented on top of cookies. When we invoke `request.get_session()`, Cjango will check if there exists a `HttpSession` object for the browser client. If not, it creates one and give the newly created `HttpSession` object a unique id. Then, it sends the unique id to the browser as a cookie. Now, every time the browser visits a page on our server, it will tell Cjango this unique id which Cjango can use to retrieve `HttpSession` for us.
