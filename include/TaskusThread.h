#ifndef TASKUS_THREAD_H
#define TASKUS_THREAD_H

#include <array>
#include <thread>
#include <deque>


#include "ThreadMessage.h"

namespace Taskus{

    class TaskusThread{
        public:
            TaskusThread(std::string nname, std::array<InterThreadQueue*,2> * nqueues);

            void loop();
            
            inline std::string getThreadName(){return name;};

        private:
            std::string name;
            
            //index 0 represents TaskusPool->TaskusThread, index 1 represents TaskusThread->TaskusPool
            std::array<InterThreadQueue*,2> * queues;
    };
}

#endif