#include "TaskusPool.h"

namespace Taskus{


TaskPool::TaskPool(){
    unsigned int n = std::thread::hardware_concurrency(); //get maximum of threads possible
    std::cout << "Number of maximum threads: " << n << "\n";
    for(int i = 0; i < n; i++){
        threadDeques.resize(threadDeques.size() + 1);
        threadDeques[threadDeques.size()-1] = new InterThreadQueue();

        TaskusThread * t = new TaskusThread(std::to_string(i), threadDeques[threadDeques.size()-1], this); 

        threads.emplace_back(t);
        tasksRunning.push_back({});
        
    }


}


void TaskPool::start(){
    //this will start all threads that are in the list
    for(int i = 0; i < threads.size(); i++){
        threads[i]->startThread();
    }
}

void TaskPool::stop(){
    //to stop a thread, first we need to send a message to it, to break the main loop and make it joinable
    for(int i = 0; i < threads.size(); i++){
        //first we gain the lock
        threadDeques[i]->queueMutex.lock();
        //build the message
        MessageThreadQueue msg;
        msg.mType = QUIT_THREAD;
        msg.priority = 0;
        //send the message in the deque
        threadDeques[i]->queue.push_back(msg);
        threadDeques[i]->queueMutex.unlock();
        //again, we notify the thread that we have put a message, after unlocking the mutex to avoid wierd
        //parallelism issues
        threadDeques[i]->condVariable.notify_one();

        threads[i]->thisThread->join();

        std::cout << "Thread " << threads[i]->getThreadName() << " has stopped." << "\n";
        

    }
}

void TaskPool::addTaskNoValidation(Task * newTask){
    //first we will find the best thread for this task (for now it's the minimum)
    int thread_id = tasksRunning[0].size();
    for(int i = 1; i < threads.size(); i++){
        if(tasksRunning[i].size() < thread_id) thread_id = i;
    }
    //now we will add the current task to this id
    //create message
    threadDeques[thread_id]->queueMutex.lock();
    MessageThreadQueue m;
    m.mType = START_TASK;
    m.numTasks = 1;
    m.tasksToRun = newTask;
    m.priority = 0;
    //send message to thread
    threadDeques[thread_id]->queue.push_front(m);
    threadDeques[thread_id]->queueMutex.unlock();
    threadDeques[thread_id]->condVariable.notify_one();
    //add task to tasksrunnign
    tasksRunning[thread_id].push_back(newTask);

    //add the dependants
    for(int i = 0; i < newTask->dependentTasks.size(); i++)
        addTaskNoValidation(newTask->dependentTasks[i]);
}

void TaskPool::mutateTask(Task * taskToMutate){
    if(taskToMutate->mutationAddTask.size() != 0){
        //remove old dependentTasks
        taskToMutate->dependentTasks = {};
        for(int i = 0; i < taskToMutate->mutationAddTask.size(); i++){
            taskToMutate->dependentTasks.push_back(taskToMutate->mutationAddTask[i]);
            taskToMutate->mutationAddTask[i]->addDependencyTask(taskToMutate);
            
        }
        taskToMutate->mutationAddTask = {};
    }
    //TODO: remove mutation task

    for(int i = 0; i < taskToMutate->dependentTasks.size(); i++){
        mutateTask(taskToMutate->dependentTasks[i]);
    }
}


void TaskPool::addTask(Task * newTask){
    //before adding to the list we need to traverse the TaskTree and add every task manually
    if(ValidateTask(newTask) != VALIDATION_PASSED){ //it will check the task in current state
        std::cout << "Couldn't validate this task, will not add it to the pool" << "\n";
        return;
    }
     
    addTaskNoValidation(newTask);
}

}
