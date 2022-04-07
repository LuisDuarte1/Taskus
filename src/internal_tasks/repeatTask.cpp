#include "internal_tasks/repeatTask.h"
#include "TaskusPool.h"



namespace Taskus{
    internalRepeatTask::internalRepeatTask(Task * nstartTask, TaskPool * nmPool) : Task(){
        startTask = nstartTask;
        mPool = nmPool;
        findEndTasks(startTask);
    }

    void internalRepeatTask::findEndTasks(Task * stask){
        if(stask->dependentTasks.size() == 0){
            bool found = false;
            for(int i = 0; i < endTasks.size(); i++){
                if(endTasks[i] == stask){
                    found = true;
                    break;
                }
            }
            if(!found) endTasks.push_back(stask);
            return;
        }
        for(int i = 0; i < stask->dependentTasks.size(); i++){
            findEndTasks(stask->dependentTasks[i]);
        }
    }

    void internalRepeatTask::runTaskFunction(){
        for(int i = 0; i < endTasks.size(); i++){
            endTasks[i]->waitToFinish();
        }
        mPool->addTask(startTask);
    }

    void internalRepeatTask::tryMutate(){

    }
}