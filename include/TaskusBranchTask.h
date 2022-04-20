#ifndef TASKUS_BRANCH_TASK_H
#define TASKUS_BRANCH_TASK_H

#include <iostream>
#include <vector>
#include <functional>

#include "TaskusTask.h"

namespace Taskus{
    class TaskPool;

    class BranchTask : public Task{
        public:
            BranchTask(TaskPool * nmasterPool);
            virtual int chooseBranch() = 0;
            void addBranchToTaskPool();
            inline size_t getAmountOfBranches(){return possibleBranches.size();};
            std::vector<std::vector<Task *>> possibleBranches;
            void addTaskToBranch(int n, Task * t);
        private:             
            bool rootIsRepeating = false;
            Task * findRootIsRepeating(Task* t);
            std::vector<Task*> findBranchEnd(int n);

            
            TaskPool * masterPool;
    };
}

#endif
