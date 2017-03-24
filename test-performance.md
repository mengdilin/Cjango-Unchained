## Baseline

```
# On Mac OS 10.11 (El Capitan)

pip install django
django-admin startproject benchmark && cd benchmark

# my mac installed python in a strange place
PYTHONPATH=/opt/intel/intelpython35/lib/python3.5/site-packages/ python manage.py runserver

```

```

# on another terminal
ab -n 1000 -c 1 http://127.0.0.1:8000/
# 1000 requests with no concurrency

This is ApacheBench, Version 2.3 <$Revision: 1706008 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking 127.0.0.1 (be patient)


Server Software:        WSGIServer/0.1
Server Hostname:        127.0.0.1
Server Port:            8000

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
ab -n 1000 -c 10 http://127.0.0.1:8000/
# 1000 requests with 10 concurrent requests

This is ApacheBench, Version 2.3 <$Revision: 1706008 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking 127.0.0.1 (be patient)


Server Software:        WSGIServer/0.1
Server Hostname:        127.0.0.1
Server Port:            8000

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