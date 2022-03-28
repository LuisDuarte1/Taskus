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

void TaskPool::addTask(Task * newTask){
    //before adding to the list we need to traverse the TaskTree and add every task manually
    //TODO(luisd): validate TaskTree acordingly, if it's invalid throw an error.
}

}
