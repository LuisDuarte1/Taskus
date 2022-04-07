#ifndef REPEAT_TASK_H
#define REPEAT_TASK_H


#include "../TaskusTask.h"


//TODO (luisd): maybe implement caching of some sorts to not have to create a task every single time
//and do the processing again. (helpful in task loops, for example)
//the end tasks may never change with mutations so it won't affect anything
namespace Taskus{
class TaskPool;

class internalRepeatTask : public Task{
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