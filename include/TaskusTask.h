#ifndef TASKUS_TASK_H
#define TASKUS_TASK_H


#include <iostream>
#include <vector>
#include <chrono>
#include <mutex>
#include <thread>
#include <atomic>
#include <semaphore>


#define MAX_TIME_ARRAY_SIZE 1000

#define MAX_DEPENDENT_TASKS 32



namespace Taskus{

    class TaskPool;

    //this task represents the base class for any class and every new task should have this as his parent
    class Task{

        public:
            Task();
            virtual ~Task();

            void runTask();

            virtual void tryMutate() = 0;


            //this function can't have any arguments, if it needs some kind of arguments, we need to
            //create a child class that has those parameters (because it's a task with a different functionality, probably)
            //this function HAS to be overriden on the child class 
            //Must change finished to true in the end
            virtual void runTaskFunction() = 0; 

            

            //a task is considered final when it has zero dependentTasks
            std::vector<Task*> dependentTasks; 


            //if a task is supposed to call itself after it's done, this parameter should only be set
            //on the tasks with depth=0 (with no dependencies), which means that after we run the root
            //task and every other task, the TaskPool will ensure that everything is executed again.
            std::atomic<bool> isRepeatable{false};

            //we need to add dependencies because the task can only run when the atomic bool finished changes to true on 
            //all of dependencies
            inline void addDependencyTask(Task * dependency){dependenciesTasks.push_back(dependency);};
            inline size_t getDependenciesSize(){return dependenciesTasks.size();};

            void waitToFinish();

            /* In order to fix this we allow for mutations, that apply on the next iteration of the
            of the task (if_repeatable is true on the root) 
            */
            std::vector<Task*> mutationAddTask;
            std::vector<Task*> mutationRemoveTask;

            //it's just a identifier for debugging, and identifing if it was added later, nothing related to the functionallity of taskus
            bool isMutation = false; 

            //median should really only work when isRepeatable is true on the root task
            uint64_t getMedianExecutedTime();


            std::vector<Task*> dependenciesTasks;

            //flag to prevent accident deletion in internalTasks
            std::atomic<bool> taskValid{true};



            void operator+=(Task * t1);

        protected:


            /*On why I removed the concept of sub-tasks altogether, it really doesnt make sense in the queue system
                let's say I add 4 tasks on 1 cpus, and task 4 is dependent on task 3 which in turn is 
                dependent in task 2 and finally dependent in task 1.
                while adding sub-tasks let's say on task 1, there is going to be, problably,
                a queue race between tasks and sub-tasks and even, there is a chance that when I add
                subtasks, the other 3 threads are already waiting for this one to finish, so it's 
                a deadlock. Therefore, to replace this functionality, which is main useful in repeatable
                threads, we can always mutate the task thread and add more stuff to it.

                MUTATIONS replace it
            */




            
            std::atomic<bool> finished{false};

            //time will be in microseconds for more accuracy
            unsigned long long timesTookExecuteTask[MAX_TIME_ARRAY_SIZE] = {};
            size_t timesSize = 0;
            inline uint64_t getLastExecutedTime(){if(timesSize>1) return timesTookExecuteTask[timesSize-1];};

            std::chrono::time_point<std::chrono::high_resolution_clock> start;
            std::chrono::time_point<std::chrono::high_resolution_clock> stop;
    };
}

#endif