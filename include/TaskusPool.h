#ifndef TASKUS_POOL_H
#define TASKUS_POOL_H

#include <iostream>
#include <thread>
#include <vector>
#include <array>
#include <string>
#include <list>
#include <unordered_map>
#include <stdexcept>

#include "ThreadMessage.h"
#include "TaskValidator.h"
#include "TaskusThread.h"
#include "internal_tasks/repeatTask.h"
#include "TaskusTask.h"
#include "internal_tasks/InternalTaskManager.h"
#include "TaskusBranchTask.h"
#include "FunctionTask.h"

namespace Taskus{

    //Task pool is the brain of the operation of this task "engine", it controls the threads and sends tasks
    //into them.
    class TaskPool{
        public:
            TaskPool();
            TaskPool(int nthreads);
            ~TaskPool();
            
            void start(); 
            void stop(); //blocking function, waits for all threads to join 

            void addTask(Task * newTask);

            Task * tryObtainNewTask(int threadId);

            void addRepeatingTask(Task * startTask, std::vector<Task*> endTasks);

            inline static int getMaxNumOfThreads(){return std::thread::hardware_concurrency();};

            inline int getNumOfThreads(){return threads.size();};


        private:

            void addTaskNoValidation(Task * newTask);

            void mutateTask(Task * taskToMutate);

            std::mutex tasksToRunMutex;
            std::list<Task *> tasksToRun;

            std::vector<TaskusThread * > threads;

            InternalTaskManager * internalCache;
            
            //next thread id to assign to a task that needs to run in same function
            int nextThreadID = 0;

            //for every thread it should be created one deques to allow interthread communication
            //it represents communication in direction TaskPool -> TaskusThread, the other direction should be
            //made with functions referencing this class 
            std::vector<InterThreadQueue *> threadDeques;
        
    };



}

#endif