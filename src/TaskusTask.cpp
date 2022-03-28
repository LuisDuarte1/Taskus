#include "TaskusTask.h"

namespace Taskus{
    Task::Task(){
        finished.store(false);
    }

    void Task::runTask(){
        #ifdef PROFILING_ENABLED
            start = std::chrono::high_resolution_clock::now();
            runTaskFunction();
            stop = std::chrono::high_resolution_clock::now();
            timesTookExecuteTask.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(stop-start).count());
        #else
            runTaskFunction();
        #endif
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

    
}