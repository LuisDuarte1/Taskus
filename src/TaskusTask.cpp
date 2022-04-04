#include "TaskusTask.h"

namespace Taskus{
    Task::Task(){
        finished = false;
    }


    void Task::runTask(){
        //wait for dependencies
        for(int i = 0; i < dependenciesTasks.size(); i++){
            dependenciesTasks[i]->waitToFinish();
        }
        //run function
        #ifdef PROFILING_ENABLED
            start = std::chrono::high_resolution_clock::now();
            runTaskFunction();
            stop = std::chrono::high_resolution_clock::now();
            timesTookExecuteTask.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(stop-start).count());
        #else
            runTaskFunction();
        #endif
        //notify dependants that this has finished
        runningMutex.lock();
        finished = true;
        runningCV.notify_all();
        runningMutex.unlock();

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
        std::unique_lock<std::mutex> lk(runningMutex);
        if(finished == true){
            return; //we first check if it has finished before waiting
        }
        runningCV.wait(lk);
        if(finished == false){ 
            //this is a spurious wakeup, which means that the thread recived a wake up call
            //but the condition isn't met
            //if this happens, we call the funcition again
            lk.unlock();
            waitToFinish();
        }
    }

    
}

