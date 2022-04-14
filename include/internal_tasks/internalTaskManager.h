#ifndef INTERNAL_TASK_CACHE_H
#define INTERNAL_TASK_CACHE_H

#include <iostream>
#include <list>
#include <iterator>

#include "internalTask.h"


#define MAX_CACHE_SIZE 10

namespace Taskus{
    class InternalTaskManager{
        public:
            InternalTaskManager();
            ~InternalTaskManager();
            //return true or false, for removed last element or not
            bool InsertInternalItem(internalTask ** newItem);
        
        private:
            std::list<internalTask *> cache;
            int FindInternalTask(internalTask **t);
    };
}


#endif
