#include "internal_tasks/repeatTask.h"
#include "TaskusPool.h"



namespace Taskus{
    internalRepeatTask::internalRepeatTask(Task * nstartTask, TaskPool * nmPool) : internalTask(){
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

    char * internalRepeatTask::cachingFunction(int * n){
        //put size of char array
        *n = (int) sizeof(startTask);  
        char * toReturn = new char[*n];
        //this is wierd by allows to convert from a Task pointer pointer to a 
        //char pointer to get the address of the pointer itself, for caching
        char * it = (char *) ((void *) &startTask);
        for(int i = 1; i < *n; i++){
            toReturn[i] = *it;
            it = it + 1;
        } 
        return toReturn;
    }
}