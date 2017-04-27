## Writing your first Cjango Application

This tutorial teaches you how to write a simple web application that displays a html page when visiting a path. 

##Directory Structure Overview
You will notice that this directory contains four folders: `callbacks`, `json`, `static`, and `templates`. `callbacks` directory contains our application codes and their compiled `.so`. `templates` directory contains all html files our application codes will be using. `static` contains all `.js`, `.css`, `.png`, and etc resources that our html pages reference to. `json` directory contains two files: `settings.json` and `urls.json`. `urls.json` provides a mapping from a http path to our application callback functions. `settings.json` defines environment variables that tells Cjango the location of `callbacks`, `json`, `static`, and `templates` directories. 

## Step 1: Write a callback
Inside `callbacks` directory, create a file called `cjango-welcome.cpp` and write our first callback function:

```c++
#include <cjango>

extern "C" http::HttpResponse render_official_site(http::HttpRequest request) {
  return http::HttpResponse::render_to_response("official.html", request);
}
```
This callback function will return a html page in `apps/http-get-demo/templates/official.html` as a http response to the client's browser. Notice that we only specified `official.html` and somehow Cjango will be able to figure out the location of `official.html`.

## Step 2: Define url mapping
We now need to tell Cjango the http path `render_official_site` corresponds to so that when a client visits a path, we can serve `official.html` page to the client. This is done by modifying `json/urls.json`. Inside `json/urls.json`, 


```
{
  "/cjango" : {
    "file" : "official-site.so",
    "funcname": "render_official_site"
  }
}
```
This lets Cjango know that when the user visits `/cjango`, run `render_official_site` as its callback function. You have noticed that `file` parameter specifies a `official-site.so` file, not `official-site.cpp` file. Let's compile compile `official-site.cpp` into `official-site.so` by running `make -C callbacks`. (We have conveniently provided you with a generic `Makefile` under `callbacks/` which you can use to compile all your application codes.

## Step 3: Set up settings configuration
There is one more step before we can run our http server. Remember how `render_official_site` only specifies the html file name without telling Cjango the exact location of the html file? Well, we have to tell Cjango the locations of our `callbacks`, `template`, `json`, and `static` folders in `json/settings.json`.

```
{
  "STATIC_URL": "apps/http-get-demo/static/",
  "TEMPLATES": "apps/http-get-demo/templates/",
  "CALLBACKS": "apps/http-get-demo/callbacks/",
  "URLS_JSON": "apps/http-get-demo/json/"
}
```
These paths are relative to the upper level directory where file `runapp` is installed.
Cjango will look for our application codes (definitions of callback functions) inside `apps/http-get-demo/callbacks/`, and html files specified in our application codes under `apps/http-get-demo/templates/`.
## Step 4: Run
Now, we are ready to run our simple http server. Go back to the top level directory and run:

```
python manage.py runserver 3000 --settings /home/ubuntu/synced_folder/c++/Cjango-Unchained/apps/http-get-demo/json/settings.json 
```

## Dynamic Reloading
Cjango can dynamically reload changes made to `json/urls.json` and `json/settings.json`. Say we want to add a new path corresponding to a new callback function, but our server is already up and running! Is there anyway to deploy our new change without interruption to the service of the server? Easy: re-compile our application codes to create new `.so` file that contains your new callback function. Update our `json/urls.json` to add a new path to our new callback function. What if we want to change the location of our `templates` directory as server is running? No problem: just change the value of `TEMPLATES` in `json/settings.json`. The changes will be read instantly without any downtime. 

## Json Response

Cjango can also return Json response by `json_str()`. See `callbacks/json-example.app`.
