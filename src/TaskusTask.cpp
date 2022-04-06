#include "TaskusTask.h"

namespace Taskus{
    Task::Task(){

    }


    void Task::runTask(){
        //wait for dependencies
        for(int i = 0; i < dependenciesTasks.size(); i++){
            dependenciesTasks[i]->waitToFinish();
        }
        //TODO: before running function aquire them all
        //run function
        #ifdef PROFILING_ENABLED
            start = std::chrono::high_resolution_clock::now();
            runTaskFunction();
            stop = std::chrono::high_resolution_clock::now();
            timesTookExecuteTask.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(stop-start).count());
        #else
            runTaskFunction();
        #endif

        for(int i = 0; i < MAX_DEPENDENT_TASKS; i++){
            finishedSemaphore.release();
        }


        

    }
    
    #ifdef PROFILING_ENABLED
        uint64_t Task::getMedianExecutedTime(){
            uint64_t m = 0;
            for(int i = 0; i < timesTookExecuteTask.size(); i++){
                m += timesTookExecuteTask[i];
            }
            m /= timesTookExecuteTask.size();
            return m;
        }


    #endif
    void Task::waitToFinish(){
        //TODO: timeout?
        finishedSemaphore.acquire();
        finishedSemaphore.release();

    }

    void Task::operator+=(Task * t1){
        this->dependentTasks.push_back(t1);
        t1->addDependencyTask(this);
    }


    

    
}

