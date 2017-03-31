## Our program (handling 1 HttpRequest by <0.3ms)

```
git checkout merge-router-and-app
make testrun
./testrun
#  add a new rule: /abc
#  [ app/app.cpp -> run -> 63 ] Invoked for port: 8080
#
# on another terminal
ab -n 1000 -c 1 http://127.0.0.1:8081/
This is ApacheBench, Version 2.3 <$Revision: 1706008 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking 127.0.0.1 (be patient)
Completed 100 requests
Completed 200 requests
Completed 300 requests
Completed 400 requests
Completed 500 requests
Completed 600 requests
Completed 700 requests
Completed 800 requests
Completed 900 requests
Completed 1000 requests
Finished 1000 requests


Server Software:
Server Hostname:        127.0.0.1
Server Port:            8081

Document Path:          /
Document Length:        96 bytes

Concurrency Level:      1
Time taken for tests:   0.267 seconds
Complete requests:      1000
Failed requests:        0
Total transferred:      140000 bytes
HTML transferred:       96000 bytes
Requests per second:    3743.87 [#/sec] (mean)
Time per request:       0.267 [ms] (mean)
Time per request:       0.267 [ms] (mean, across all concurrent requests)
Transfer rate:          511.86 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    0   0.1      0       1
Processing:     0    0   0.0      0       1
Waiting:        0    0   0.0      0       1
Total:          0    0   0.1      0       1

Percentage of the requests served within a certain time (ms)
  50%      0
  66%      0
  75%      0
  80%      0
  90%      0
  95%      0
  98%      0
  99%      1
 100%      1 (longest request)
```

## Baseline (django handles 1 HttpRequest by ~2.5ms)

```
# On Mac OS 10.11 (El Capitan)

pip install django
django-admin startproject benchmark && cd benchmark

# my mac installed python in a strange place
# PYTHONPATH=/opt/intel/intelpython35/lib/python3.5/site-packages/ python manage.py runserver 13579
PYTHONPATH=/opt/intel/intelpython35/lib/python3.5/site-packages/ python manage.py runserver 13579  > /dev/null 2>&1
```

```

# on another terminalab -n 1000 -c 1 http://127.0.0.1:13579/
This is ApacheBench, Version 2.3 <$Revision: 1706008 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking 127.0.0.1 (be patient)
Completed 100 requests
Completed 200 requests
Completed 300 requests
Completed 400 requests
Completed 500 requests
Completed 600 requests
Completed 700 requests
Completed 800 requests
Completed 900 requests
Completed 1000 requests
Finished 1000 requests


Server Software:        WSGIServer/0.1
Server Hostname:        127.0.0.1
Server Port:            13579

Document Path:          /
Document Length:        1767 bytes

Concurrency Level:      1
Time taken for tests:   2.002 seconds
Complete requests:      1000
Failed requests:        0
Total transferred:      1915000 bytes
HTML transferred:       1767000 bytes
Requests per second:    499.53 [#/sec] (mean)
Time per request:       2.002 [ms] (mean)
Time per request:       2.002 [ms] (mean, across all concurrent requests)
Transfer rate:          934.18 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    0   0.0      0       1
Processing:     1    2   8.8      1     277
Waiting:        1    1   1.0      1      25
Total:          1    2   8.8      1     277

Percentage of the requests served within a certain time (ms)
  50%      1
  66%      2
  75%      2
  80%      2
  90%      2
  95%      4
  98%      5
  99%      5
 100%    277 (longest request)

```