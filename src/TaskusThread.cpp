#include "TaskusThread.h"
#include "TaskusPool.h"


namespace Taskus{
    TaskusThread::TaskusThread(std::string nname, InterThreadQueue * recv_queue, TaskPool * tPool)
    {
        name = nname;
        receiveQueue = recv_queue;
        masterPool = tPool;
        
        
    }

    void TaskusThread::loop(){
        while(true){
            std::unique_lock<std::mutex> lk{receiveQueue->queueMutex};
            receiveQueue->condVariable.wait(lk);
            //after this we gain the lock on the queueMutex and we can alter everything we want
            MessageThreadQueue m = receiveQueue->queue.at(0);
            receiveQueue->queue.pop_front();

            lk.unlock();
            //we unlock prior to notifying because it doesnt make that much sense and it could run into some
            //conflits ig
            receiveQueue->condVariable.notify_one();


            //now we can process the message freely
            bool quit = false;
            switch (m.mType)
            {
            case QUIT_THREAD:
                quit = true;
                break;
            case START_TASK:
                for(int i = 0; i < m.numTasks; i++){
                    //TODO: run task can kind of mutate the task itself (add more tasks between itself and its dependants)
                    //but it will only be effective in the next run (if there's any)
                    m.tasksToRun[i].runTask(); 
                    //TODO: remove task from running tasks in master pool
                    
                }
                break;
            default:
                break;
            }
            if(quit) break; //exit the main loop which makes the thread joinable by the system
        }
    }

    void TaskusThread::startThread(){
        thisThread = new std::thread([this]{loop();});
    }
}