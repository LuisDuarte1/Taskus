#ifndef TASKUS_THREAD_H
#define TASKUS_THREAD_H

#include <array>
#include <thread>
#include <deque>


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

        private:
            std::string name;
            
            
            InterThreadQueue * receiveQueue;
            TaskPool * masterPool;
    };
}

#endif