# Results

![](performance.png)

# Overview

+ Executed the following commands from a 1.2GHz single board computer (Raspberry Pi 3)
  + `nc=${num}; name=cjango-c${nc}-1; ab -n 10000 -c ${nc} -g ${name}.log http://192.168.0.15:8000/ > ${name}.txt`
  + `${num}` is set as 1, 3, 5, 7, 10, 50, 100, 150
    + Django cannot handle >=15 concurrent requests (`connection reset by peer` error)
    + All C++ libraries cannot handle 200 concurrent requests (the same as above)
  + all raw log files are stored under `bench/ethernet/`

# Experimental Conditions

#### Server Spec

  + Macbook Pro Mid 2014
    + OS X 10.11.6 (El Capitan)
    + 3GHz Intel Core i7 Dual Core
    + 16GB DDR3 memory

#### Client Spec

  + [Raspberry Pi 3 Model B+](https://www.raspberrypi.org/products/raspberry-pi-3-model-b/)

#### Network settings
  + (Raspberry Pi 3) <- 100Mbps ethernet (RPI3's maximum) -> Router <- 1Gbps Ether-Thunderbolt adapter -> Macbook

#### App Framework Settings

  + remove all URL mappings except one (root dir `/` -> a function to print out `"HelloWorld"`)
    + silicon/django uses `/hello/` as URL
  + disable all debugging messages for all app frameworks
  + use the same port (8000)
  + all compiled by the same compiler `Apple LLVM version 7.0.2 (clang-700.1.81)`