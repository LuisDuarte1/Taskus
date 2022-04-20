#include "TaskusBranchTask.h"
#include "TaskusPool.h"

namespace Taskus{
    BranchTask::BranchTask(TaskPool * nmasterPool){
        masterPool = nmasterPool;
    }

    void BranchTask::addBranchToTaskPool(){
        int desired_branch = chooseBranch();
        for(int i = 0; i < possibleBranches[desired_branch].size(); i++){
            masterPool->addTask(possibleBranches[desired_branch][i]);
        }
        if(isRepeatable.load()){
            //if this task is repeatable in itself, we find the end of the desired branch and repeat this task again until is_repeating is disabled
            std::vector<Task*> endBranchTasks = findBranchEnd(desired_branch);
            masterPool->addRepeatingTask(this, endBranchTasks);
        } else{
            Task * root = findRootIsRepeating(this);
            if(rootIsRepeating){
                //if this task doesnt repeat root has is_repeating on, we jump to root again on the end of this branch
                std::vector<Task*> endBranchTasks = findBranchEnd(desired_branch);
                masterPool->addRepeatingTask(root, endBranchTasks);
            }
        }
    }

    Task * BranchTask::findRootIsRepeating(Task* t){
        //TODO: memoization to saves resources and the memory wasted is not that big
        if(t->getDependenciesSize() == 0){
            rootIsRepeating = t->isRepeatable.load();
            return t;
        }
        return findRootIsRepeating(t->dependenciesTasks[0]);
    }


    std::vector<Task*> BranchTask::findBranchEnd(int n){
        //instead of nested functions in python we can use lambda functions 
        //recursion function to find all task ends

        //TODO: memoization again to save resources and the memory wasted is not that big

        std::function<std::vector<Task*> (Task *)> recursion_function = [&](Task * t) -> std::vector<Task*>{
            if(t->dependentTasks.size() == 0) {
                //don't append branch tasks because they cannot be the end of the task (they have to always choose a path)
                BranchTask * try_branch_task = dynamic_cast<BranchTask*>(t);
                if(try_branch_task != nullptr) return {};
                return {t};
            }
            std::vector<Task *> endTasks;
            for(int i = 0; i < t->dependentTasks.size(); i++){
                std::vector<Task *> toAppendEndTasks = recursion_function(t->dependentTasks[i]);
                for(int e = 0; e < toAppendEndTasks.size(); e++){
                    bool found = false;
                    for(int u = 0; u < endTasks.size(); u++){
                        if(endTasks[u] == toAppendEndTasks[i]){
                            found = true;
                            break;
                        }
                    }
                    //only add if it's not a duplicate
                    if(!found) endTasks.push_back(toAppendEndTasks[e]);
                }
            }
            return endTasks;
        };


        std::vector<Task *> endTasks;
        for(int i = 0; i < possibleBranches[n].size(); i++){
            std::vector<Task *> toAppendEndTasks = recursion_function(possibleBranches[n][i]);
            for(int e = 0; e < toAppendEndTasks.size(); e++){
                bool found = false;
                for(int u = 0; u < endTasks.size(); u++){
                    if(endTasks[u] == toAppendEndTasks[i]){
                        found = true;
                        break;
                    }
                }
                //only add if it's not a duplicate
                if(!found) endTasks.push_back(toAppendEndTasks[e]);
            }
        }
        return endTasks;
    }

    void BranchTask::addTaskToBranch(int n, Task * t){
        if(n > (static_cast<int>(possibleBranches.size())-1)){
            //if n > than all indexes in possible branches we create a list until we reach that number
            for(int i = 0; i <= (n - (possibleBranches.size()-1)); i++){
                possibleBranches.push_back({});
            }
        }
        possibleBranches[n].push_back(t);
    }


}

