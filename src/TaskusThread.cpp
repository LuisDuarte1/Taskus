#include "TaskusThread.h"


namespace Taskus{
    TaskusThread::TaskusThread(std::string nname, std::array<InterThreadQueue*,2> * nqueues){
        name = nname;
        queues = nqueues;
        
        
    }

    void TaskusThread::loop(){
        while(true){
            std::unique_lock lk(queues->at(0)->queueMutex);
            queues->at(0)->condVariable.wait(lk);
            //after this we gain the lock on the queueMutex and we can alter everything we want
            MessageThreadQueue m = queues->at(0)->queue.at(0);
            queues->at(0)->queue.pop_front();

            lk.unlock();
            //we unlock prior to notifying because it doesnt make that much sense and it could run into some
            //conflits ig
            queues->at(0)->condVariable.notify_one();


            //now we can process the message freely
            bool quit = false;
            switch (m.mType)
            {
            case QUIT_THREAD:
                quit = true;
                break;
            
            default:
                break;
            }
            if(quit) break; //exit the main loop which makes the thread joinable by the system
        }
    }
}