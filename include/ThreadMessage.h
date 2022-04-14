#ifndef THREAD_MESSAGE_H
#define THREAD_MESSAGE_H

#include <iostream>
#include <deque>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include "TaskusTask.h"

namespace Taskus{


    enum MessageType{
        TASK_AVAILABLE,
        QUIT_THREAD 
    };

    struct MessageThreadQueue{
        MessageType mType; //This is obrigatory in every message, every other argument could be optional
        uint8_t priority = 0; //0-20. 0 means lowest priority, 20 means highest priority
    };


    struct InterThreadQueue{
        std::mutex queueMutex;
        std::deque<MessageThreadQueue> queue;
        std::condition_variable condVariable;
    };
}





#endif