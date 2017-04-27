## Writing your first Cjango Application

This directory contains an example Cjango application of HTTP `GET` method.
The difference from hello-world example is this example uses **static file directory** and **template file directory**.

# How does Cjango serve HTTP Response?

```json
# apps/http-get-demojson/urls.json

{
  "/" : {
    "file" : "cjango-welcome.so",
    "funcname": "render_cjango_welcome_page"
  },
  "/cjango" : {
    "file" : "official-site.so",
    "funcname": "render_official_site"
  },
  "/json" : {
    "file" : "json-example.so",
    "funcname": "return_hello_json"
  }
}

```

Let's look at `render_cjango_welcome_page` callback function defined in `cjango-welcome.cpp`.

##

```cpp
// In cjango-welcome.cpp

#include <cjango>

extern "C" http::HttpResponse render_cjango_welcome_page(http::HttpRequest request) {
  return http::HttpResponse::render_to_response("../templates/welcome.html");
}
```

This callback function `render_cjango_welcome_page` serves a pre-written html file by `http::HttpResponse::render_to_response()`. `std::string http::HttpResponse::get_template(http::HttpRequest)` can be used for serving a fragment of a complete html file.

Note: The relative path is referenced **not from this cpp source file, but from the `TEMPLATES` parameter you specified in your `settings.json` file. In this example, the `settings.json` is set as follows:

```json
{
  "STATIC_URL": "apps/http-get-demo/static/",
  "TEMPLATES": "apps/http-get-demo/templates/",
  "CALLBACKS": "apps/http-get-demo/callbacks/",
  "URLS_JSON": "apps/http-get-demo/json/"
}

```

Thus, the redundant path specification `render_to_response("../templates/welcome.html")` can be written as:

```cpp
// apps/htto-get-demo/callbacks/cjango-welcome.cpp

#include <cjango>

extern "C" http::HttpResponse render_cjango_welcome_page(http::HttpRequest request) {
  return http::HttpResponse::render_to_response("welcome.html");
}

```

If you want to use app-wise template directory, simply change the above `TEMPLATES` value from `"apps/http-get-demo/templates/"` to `"templates/"` (i.e. `Cjango-Unchained/templates`). This change can be done at runtime, so if you have two directories such as `templates/` and `templates-v2/`, if you change your `settings.json`, the changes will be read instantly without any downtime.



## Json Response

Cjango can also return Json response by `json_str()`. See `callbacks/json-example.app`.
