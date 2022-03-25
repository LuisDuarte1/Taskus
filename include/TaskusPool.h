#ifndef TASKUS_POOL_H
#define TASKUS_POOL_H

#include <iostream>
#include <thread>
#include <vector>
#include <array>
#include <string>

#include "ThreadMessage.h"
#include "TaskusThread.h"

namespace Taskus{

    //Task pool is the brain of the operation of this task "engine", it controls the threads and sends tasks
    //into them.
    class TaskPool{
        public:
            TaskPool();
            //TODO: start and quit functions
            void start(); 
            void stop(); //blocking function, waits for all threads to join 

        private:

            std::vector<TaskusThread * > threads;

            //for every thread it should be created two deques to allow interthread communication
            //index 0 represents TaskusPool->TaskusThread, index 1 represents TaskusThread->TaskusPool
            std::vector<std::array<InterThreadQueue*,2>> thread_deques;

    };



}

#endif