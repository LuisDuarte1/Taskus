#include "TaskusTask.h"

namespace Taskus{
    Task::Task(){

    }

    void Task::runTask(){
        //wait for dependencies
        for(int i = 0; i < dependenciesTasks.size(); i++){
            dependenciesTasks[i]->waitToFinish();
        }

        finished.store(false);
        start = std::chrono::high_resolution_clock::now();
        runTaskFunction();
        stop = std::chrono::high_resolution_clock::now();
        unsigned long long e = std::chrono::duration_cast<std::chrono::microseconds>(stop-start).count();
        if(timesSize == MAX_TIME_ARRAY_SIZE){
            //reset array if the max has reached
            for(int i = 0; i < timesSize; i++){
                timesTookExecuteTask[i] = 0;
            }
            timesSize = 0;
        }
        timesTookExecuteTask[timesSize] = e; 
        timesSize++;
        finished.store(true);
        finished.notify_all();

        

    }
    
        uint64_t Task::getMedianExecutedTime(){
            uint64_t m = 0;
            for(int i = 0; i < timesSize; i++){
                m += timesTookExecuteTask[i];
            }
            if(timesSize = 0) return 0; //dont divide by zero lmao
            m /= timesSize;
            return m;
        }

    void Task::waitToFinish(){
        finished.wait(false);
        if(!finished.load()){
            waitToFinish();
        }

    }

    void Task::operator+=(Task * t1){
        this->dependentTasks.push_back(t1);
        t1->addDependencyTask(this);
    }



    Task::~Task(){
        
    }

    
}


