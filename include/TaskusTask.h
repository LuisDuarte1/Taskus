#ifndef TASKUS_TASK_H
#define TASKUS_TASK_H


#include <iostream>
#include <vector>
#include <chrono>
#include <atomic>

#define PROFILING_ENABLED 1


namespace Taskus{
    //this task represents the base class for any class and every new task should have this as his parent
    class Task{

        public:
            Task();


            void runTask();

            //this function can't have any arguments, if it needs some kind of arguments, we need to
            //create a child class that has those parameters (because it's a task with a different functionality, probably)
            //this function HAS to be overriden on the child class 
            //Must change finished to true in the end
            virtual void runTaskFunction() = 0; 

            //a task is considered final when it has zero dependentTasks
            std::vector<Task*> dependentTasks; 

            //funtion that can be overriden to add/remove new subtasks, the added subtasks will be ran first
            //than the main Task itself
            virtual void checkForSubTasks() = 0;

            //if a task is supposed to call itself after it's done, this parameter should only be set
            //on the tasks with depth=0 (with no dependencies), which means that after we run the root
            //task and every other task, the TaskPool will ensure that everything is executed again.
            //this is also aplicable in the subtasks (although not recommended because it can lead to a infinite loop)
            bool isRepeatable = false;

            //we need to add dependencies because the task can only run when the atomic bool finished changes to true on 
            //all of dependencies
            void addDependencyTask(Task * dependency);
        
        private:
            //subtasks are when the task itself can be divided into segments, that according to it self,
            //can be even further parellized, Taskus will not guarentee that running the subTasks is actually faster
            std::vector<Task*> subTasks;

            std::vector<Task*> dependenciesTasks;


            std::atomic<bool> finished;

            //this can be disabled in release builds to save a bit of processing
            #ifdef PROFILING_ENABLED
                std::vector<uint64_t> timesTookExecuteTask;
                inline uint64_t getLastExecutedTime(){if(timesTookExecuteTask.size()>1) return timesTookExecuteTask[timesTookExecuteTask.size()-1];};
                //median should really only work when isRepeatable is true on the root task
                uint64_t getMedianExecutedTime();
                std::chrono::time_point<std::chrono::high_resolution_clock> start;
                std::chrono::time_point<std::chrono::high_resolution_clock> stop;
            #endif
    };
}

#endif