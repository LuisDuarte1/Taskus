# Taskus

A library to simplify the use of multithreading in C++ with as minimal overhead as possible. 

(You are free to use Taskus, but [OpenMP](https://www.openmp.org/) or [Intel's TTB](https://www.intel.com/content/www/us/en/developer/tools/oneapi/onetbb.html#gs.zzssyf) might be a better alternative for HPC).

## Installation

Taskus **requires** CMake and a C++ compiler capable of using the C++20 standard (the only exception is mingw-w64, note below).

Can be built with:
```
mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE="Release"
make
```

If your project uses also CMake, you just need to add the directory of Taskus using **add_subdirectory**.


**Note:** Mingw-w64 is not supported currently because of a bug in it (I suspect it is related to winpthreads) and somes times locks the program, if you want to use Taskus on Windows consider your project to use MSVC instead.

## Usage
This Example uses Function Tasks, which might not be ideal for your application see the [Quick-Start](docs/QuickStart.md) for more information.
```cpp
#include <iostream>
#include <Taskus.h>


int main(){
    Taskus::TaskPool tPool;  //this assumes max threads
    tPool.start();
    std::vector<int> aa = {1,2,3,4,5,6,7,8,9,10};
    std::vector<int> bb = {10,9,8,7,6,5,4,3,2,1};
    auto ftask = Taskus::makeFunctionTask<int>([](int a, int b){return a*b/2;},
        aa,bb);
    ftask->divideTasks();
    tPool.addTask(ftask->startTask);
    tPool.stop();
    std::cout << "Results:\n";
    for(int a : tPool->result) std::cout << a << "\n";
    delete ftask;
    return 0;
}

```

## Features

- Task Pool
- Repeatable task trees
- Conditional task branching
- Function Task (a wrapper to simplify the creation of tasks)
- Task Mutation (not yet fully implemented)
- Task Format Validation

## To-Do List

- Task Remove Mutation
- Syncronization Objects
- Better Task Scheduler instead of a FIFO list.
- Profiller
- Task visualization tool

