#ifndef TASKUS_THREAD_H
#define TASKUS_THREAD_H

#include <array>
#include <thread>
#include <deque>
#include <atomic>


#include "ThreadMessage.h"

namespace Taskus{
    class TaskPool;


    class TaskusThread{
        public:
        
            TaskusThread(std::string nname, InterThreadQueue * recv_queue, TaskPool * tPool);

            void startThread();

            std::thread * thisThread;

            void loop();
            
            inline std::string getThreadName(){return name;};
            inline bool getIfBusy(){return is_busy.load();};


        private:
            std::string name;
            
            bool getIfReceivedMessage();
            
            InterThreadQueue * receiveQueue;
            TaskPool * masterPool;
            std::atomic<bool> is_busy;

    };
}

#endif