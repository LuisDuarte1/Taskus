# **How to write Taskus benchmarks**

## Metodology

- A Taskus benchmark should consist of two parts of the same functions, **a single threaded version** (Taskus can be used to do it, to simplify implementation, it has a bit of overhead though) and a **multi-threaded version** that takes advantage of the maximum number of cores. 

- To achieve this every benchmark program should have two arguments `--single` and `--multi` that defines the behaviour of the program.

- A benchmark should **NOT** repeat itself. (the purpose of this is to simplify analysis for instance: *VTune* when we run multiple tests/benchmarks, it will not divide the program and will make analysis harder) This should be accomplished by a profiler to run the program an **n** amount of times (like *VTune* or another profiling program)

- Try to write benchmarks that **put the same load on all threads**. For instance the *rendermandelbrotsetimage* bench test even though it's symetric in the x axis, it's not in the y axis which means that the left side of the image will render way more faster than the right side, which means that the left side threads when they finish will do nothing. (which is a consequence of the algorithm itself, not Taskus) and we have interest in finding the Taskus internal bottlenecks no the benchmarking algorithms.

## Reasons to write benchmarks for Taskus

- When this benchmarks are done with a profiler, makes writing optimizations a tad easier for Taskus functionality.

- While also writing benchmarks we are also getting experience on the developer experience and workflow with it. Which with proper feedback will make Taskus more easier to work, while getting more efficient overtime.



