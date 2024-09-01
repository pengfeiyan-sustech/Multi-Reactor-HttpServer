# 基于多反应堆的高并发服务器

项目编译
```
cd build
cmake ..
make
```

项目运行
```
./app
```

压力测试
```
./webbench -c 100 -t 10 http://127.0.0.1:5050/

Webbench - Simple Web Benchmark 1.5
Copyright (c) Radim Kolar 1997-2004, GPL Open Source Software.

Benchmarking: GET http://127.0.0.1:5050/
100 clients, running 10 sec.

Speed=126060 pages/min, 3097156 bytes/sec.
Requests: 21010 susceed, 0 failed.
```

```
./webbench -c 1000 -t 10 http://127.0.0.1:5050/

Webbench - Simple Web Benchmark 1.5
Copyright (c) Radim Kolar 1997-2004, GPL Open Source Software.

Benchmarking: GET http://127.0.0.1:5050/
1000 clients, running 10 sec.

Speed=123474 pages/min, 3033620 bytes/sec.
Requests: 20578 susceed, 1 failed.
```

```
./webbench -c 5000 -t 10 http://127.0.0.1:5050/

Webbench - Simple Web Benchmark 1.5
Copyright (c) Radim Kolar 1997-2004, GPL Open Source Software.

Benchmarking: GET http://127.0.0.1:5050/
5000 clients, running 10 sec.

Speed=111918 pages/min, 2748920 bytes/sec.
Requests: 18651 susceed, 2 failed.
```

```
./webbench -c 10000 -t 10 http://127.0.0.1:5050/

Webbench - Simple Web Benchmark 1.5
Copyright (c) Radim Kolar 1997-2004, GPL Open Source Software.

Benchmarking: GET http://127.0.0.1:5050/
10000 clients, running 10 sec.

Speed=100266 pages/min, 2462770 bytes/sec.
Requests: 16709 susceed, 2 failed.
```
