#ifndef REPEAT_TASK_H
#define REPEAT_TASK_H


#include "internalTask.h"


namespace Taskus{
class TaskPool;

class internalRepeatTask : public internalTask{
    public:
        internalRepeatTask(Task * nstartTask, TaskPool * nmPool);
        void tryMutate();
        void runTaskFunction();

    private:
        TaskPool * mPool;
        std::vector<Task *> endTasks;
        Task * startTask;
        void findEndTasks(Task * stask);
        

        
};

}

#endif