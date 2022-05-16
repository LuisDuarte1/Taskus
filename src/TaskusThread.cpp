#include "TaskusThread.h"
#include "TaskusPool.h"


namespace Taskus{
    TaskusThread::TaskusThread(int nname, InterThreadQueue * recv_queue, TaskPool * tPool)
    {
        name = nname;
        receiveQueue = recv_queue;
        masterPool = tPool;
        
        
    }

    bool TaskusThread::getIfReceivedMessage(){
        return (receiveQueue->queue.size() > 0);
    }


    void TaskusThread::loop(){
        while(true){
            auto& tmp = receiveQueue->queue;
            std::unique_lock<std::mutex> lk{receiveQueue->queueMutex};
            while(!getIfReceivedMessage()){
                receiveQueue->condVariable.wait(lk);
            }

            //after this we gain the lock on the queueMutex and we can alter everything we want
            MessageThreadQueue m = receiveQueue->queue.at(0);
            receiveQueue->queue.pop_front();

            lk.unlock();


            //now we can process the message freely
            bool quit = false;
            switch (m.mType)
            {
            case QUIT_THREAD:
                quit = true;
                break;
            case TASK_AVAILABLE:
                Task * newTask = masterPool->tryObtainNewTask(name);
                if(newTask == nullptr) break;
                receiveQueue->threadBusy.store(true);
                newTask->tryMutate();
                newTask->runTask();
                BranchTask * tryBranch = dynamic_cast<BranchTask*>(newTask);
                if(tryBranch != nullptr){
                    tryBranch->addBranchToTaskPool();
                }
                newTask->taskValid.store(false);
                receiveQueue->threadBusy.store(false);
                receiveQueue->threadBusy.notify_all();
                break;
            }
            if(quit) break; //exit the main loop which makes the thread joinable by the system
        }
    }

    void TaskusThread::startThread(){
        thisThread = new std::thread([this]{loop();});
    }

    TaskusThread::~TaskusThread(){
        delete thisThread;
    }
}
