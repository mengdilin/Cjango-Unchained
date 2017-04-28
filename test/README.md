## TO RUN

```bash
python test.py test_post_demo.json --config config.json --lib verifications_post_demo
```

`test.py` executes the intergration tests written in `test_post_demo.json`, with configurations defined by `config.json` and additional tester functions in `verifications_post_demo.py` (note the `.py` extension should be discarded).

## Files

* `test.py`

Function that executes the test.

```bash
$ python test.py -h
usage: test.py [-h] [--lib LIB] [--config CONFIG] [--disable_color]
               test_cases_file

integration test framework (support HTTP only)

positional arguments:
  test_cases_file  json file with test cases

optional arguments:
  -h, --help       show this help message and exit
  --lib LIB        python file (under SAME DIRECTORY) with verification
                   functions
  --config CONFIG  json file with global configurations
  --disable_color  disable printing to terminal with multiple colors
```
* `test_cases.json`

Contains test cases written in json format. It allows you to specify items to include in the request and to expect in the response. For example:

```json
"case_id" : {
        "NAME" : "case_name",
        "DESCRIPTION" : "descriptions",

        "REQ_PATH" : "/home",
        "REQ_METHOD" : "POST",
        "REQ_PARAMS" : {
            "name" : "John",
            "age" : "10"        
        },
        "REQ_FILE" : "requests/request.txt",
        "REQ_HEADERS" : {
            "Accept" : "text/plain"
        },

        "RESP_STATUS" : "200",
        "RESP_REASON" : "OK",
        "RESP_VERSION" : "HTTP/1.0",
        "RESP_HEADERS" : {
            "content-type" : "text/html",
            "content-length" : "100"
        },
        "RESP_VERIFY_FUNCTION" : "own_verification_function"
    }
```

* `verifications.py`

Contains functions for specific verifications (e.g. contents of certain tags in HTML). The function will be given the data sent with response in `str` format and should return `True/False` to indicate the result of verification. The name of function should be put in the `RESP_VERIFY_FUNCTION` fields in `test_cases.json`

* `config.json`

Specifies the configurations for the test. Currently only `http` protocol is supported. For example,

```json
{
    "PROTOCOL" : "http",
    "HOST" : "127.0.0.1",
    "PORT" : "8000",
    "TIMEOUT" : "10"
}

```