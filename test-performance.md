## Our program (handling 1 HttpRequest by <0.5ms)

```
git checkout merge-router-and-app
make testrun
./testrun
#  add a new rule: /abc
#  [ app/app.cpp -> run -> 63 ] Invoked for port: 8080
#
# on another terminal
ab -n 1000 -c 1 http://127.0.0.1:8080/

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
Server Port:            8080

Document Path:          /
Document Length:        96 bytes

Concurrency Level:      1
Time taken for tests:   0.464 seconds
Complete requests:      1000
Failed requests:        0
Total transferred:      140000 bytes
HTML transferred:       96000 bytes
Requests per second:    2154.52 [#/sec] (mean)
Time per request:       0.464 [ms] (mean)
Time per request:       0.464 [ms] (mean, across all concurrent requests)
Transfer rate:          294.56 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    0   0.0      0       1
Processing:     0    0   0.3      0       6
Waiting:        0    0   0.3      0       6
Total:          0    0   0.3      0       6

Percentage of the requests served within a certain time (ms)
  50%      0
  66%      0
  75%      1
  80%      1
  90%      1
  95%      1
  98%      1
  99%      1
 100%      6 (longest request)
```

## Baseline (django handles 1 HttpRequest by ~2.5ms)

```
# On Mac OS 10.11 (El Capitan)

pip install django
django-admin startproject benchmark && cd benchmark

# my mac installed python in a strange place
PYTHONPATH=/opt/intel/intelpython35/lib/python3.5/site-packages/ python manage.py runserver 13579

```

```

# on another terminal
ab -n 1000 -c 1 http://127.0.0.1:13579/
# 1000 requests with no concurrency

This is ApacheBench, Version 2.3 <$Revision: 1706008 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking 127.0.0.1 (be patient)


Server Software:        WSGIServer/0.1
Server Hostname:        127.0.0.1
Server Port:            13579

Document Path:          /
Document Length:        1767 bytes

Concurrency Level:      1
Time taken for tests:   2.470 seconds
Complete requests:      1000
Failed requests:        0
Total transferred:      1915000 bytes
HTML transferred:       1767000 bytes
Requests per second:    404.80 [#/sec] (mean)
Time per request:       2.470 [ms] (mean)
Time per request:       2.470 [ms] (mean, across all concurrent requests)
Transfer rate:          757.03 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    0   0.1      0       2
Processing:     1    2   1.3      2      14
Waiting:        1    2   1.1      1      13
Total:          1    2   1.4      2      14

Percentage of the requests served within a certain time (ms)
  50%      2
  66%      2
  75%      3
  80%      3
  90%      4
  95%      5
  98%      6
  99%      7
 100%     14 (longest request)
```

```

# on another terminal
ab -n 1000 -c 10 http://127.0.0.1:13579/
# 1000 requests with 10 concurrent requests

This is ApacheBench, Version 2.3 <$Revision: 1706008 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking 127.0.0.1 (be patient)


Server Software:        WSGIServer/0.1
Server Hostname:        127.0.0.1
Server Port:            13579

Document Path:          /
Document Length:        1767 bytes

Concurrency Level:      10
Time taken for tests:   2.481 seconds
Complete requests:      1000
Failed requests:        0
Total transferred:      1915000 bytes
HTML transferred:       1767000 bytes
Requests per second:    403.07 [#/sec] (mean)
Time per request:       24.810 [ms] (mean)
Time per request:       2.481 [ms] (mean, across all concurrent requests)
Transfer rate:          753.79 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    0   0.1      0       2
Processing:     5   25   6.6     24      51
Waiting:        3   22   6.2     21      47
Total:          6   25   6.6     24      51

Percentage of the requests served within a certain time (ms)
  50%     24
  66%     26
  75%     28
  80%     30
  90%     34
  95%     38
  98%     41
  99%     43
 100%     51 (longest request)

```