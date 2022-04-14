#ifndef TASKUS_POOL_H
#define TASKUS_POOL_H

#include <iostream>
#include <thread>
#include <vector>
#include <array>
#include <string>
#include <list>
#include <stdexcept>

#include "ThreadMessage.h"
#include "TaskValidator.h"
#include "TaskusThread.h"
#include "internal_tasks/repeatTask.h"
#include "TaskusTask.h"
#include "internal_tasks/InternalTaskManager.h"

namespace Taskus{

    //Task pool is the brain of the operation of this task "engine", it controls the threads and sends tasks
    //into them.
    class TaskPool{
        public:
            TaskPool();
            ~TaskPool();
            
            void start(); 
            void stop(); //blocking function, waits for all threads to join 

            void addTask(Task * newTask);

            Task * tryObtainNewTask();


        private:

            void addTaskNoValidation(Task * newTask);

            void mutateTask(Task * taskToMutate);

            std::mutex tasksToRunMutex;
            std::list<Task *> tasksToRun;

            std::vector<TaskusThread * > threads;

            InternalTaskManager * internalCache;
            

            //for every thread it should be created one deques to allow interthread communication
            //it represents communication in direction TaskPool -> TaskusThread, the other direction should be
            //made with functions referencing this class 
            std::vector<InterThreadQueue *> threadDeques;
        
    };



}

#endif