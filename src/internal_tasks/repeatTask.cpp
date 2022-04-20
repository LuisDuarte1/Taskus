#include "internal_tasks/repeatTask.h"
#include "TaskusPool.h"
#include "TaskusBranchTask.h"



namespace Taskus{
    internalRepeatTask::internalRepeatTask(Task * nstartTask, TaskPool * nmPool) : internalTask(){
        startTask = nstartTask;
        mPool = nmPool;
        findEndTasks(startTask);
    }

    internalRepeatTask::internalRepeatTask(Task * nstartTask,std::vector<Task*> nendTasks, TaskPool * nmPool){
        startTask = nstartTask;
        mPool = nmPool;
        endTasks = nendTasks;
    }


    void internalRepeatTask::findEndTasks(Task * stask){
        if(stask->dependentTasks.size() == 0){
            //don't append branch tasks
            BranchTask * try_branch_task = dynamic_cast<BranchTask*>(stask);
            if(try_branch_task != nullptr) return;
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
        if(startTask->isRepeatable.load()){
            mPool->addTask(startTask);
        }
    }

    void internalRepeatTask::tryMutate(){

    }

    std::vector<char> internalRepeatTask::cachingFunction(){
        std::vector<char> r;
        char * c = (char*)((void*)&startTask);
        for(int i = 0; i < sizeof(Task*); i++){
            r.push_back(*c);
            c++;
        }

        return r;
    }



}