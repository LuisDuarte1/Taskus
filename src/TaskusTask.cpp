#include "TaskusTask.h"

namespace Taskus{
    Task::Task(){

    }

    void Task::runTask(){
        //wait for dependencies
        for(int i = 0; i < dependenciesTasks.size(); i++){
            dependenciesTasks[i]->waitToFinish();
        }
        for(int i = 0; i < MAX_DEPENDENT_TASKS; i++){
            if(!finishedSemaphore.try_acquire()){
                break;
            }
        }
        start = std::chrono::high_resolution_clock::now();
        runTaskFunction();
        stop = std::chrono::high_resolution_clock::now();
        unsigned long long e = std::chrono::duration_cast<std::chrono::milliseconds>(stop-start).count();
        if(timesSize == MAX_TIME_ARRAY_SIZE){
            //reset array if the max has reached
            for(int i = 0; i < timesSize; i++){
                timesTookExecuteTask[i] = 0;
            }
            timesSize = 0;
        }
        timesTookExecuteTask[timesSize] = e; 
        timesSize++;

        for(int i = 0; i < MAX_DEPENDENT_TASKS; i++){
            finishedSemaphore.release();
        }


        

    }
    
        uint64_t Task::getMedianExecutedTime(){
            uint64_t m = 0;
            for(int i = 0; i < timesSize; i++){
                m += timesTookExecuteTask[i];
            }
            m /= timesSize;
            return m;
        }

    void Task::waitToFinish(){
        //TODO: timeout?
        finishedSemaphore.acquire();
        finishedSemaphore.release();

    }

    void Task::operator+=(Task * t1){
        this->dependentTasks.push_back(t1);
        t1->addDependencyTask(this);
    }



    Task::~Task(){
        
    }

    
}


