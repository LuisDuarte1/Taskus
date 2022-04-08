#ifndef INTERNAL_TASK_CACHE_H
#define INTERNAL_TASK_CACHE_H

#include <iostream>
#include <list>
#include <iterator>

#include "internalTask.h"


#define MAX_CACHE_SIZE 10

namespace Taskus{
    class InternalTaskCache{
        public:
            InternalTaskCache();
            ~InternalTaskCache();
            //return true or false, for hit or miss
            bool InsertInternalItem(internalTask ** newItem);
        
        private:
            internalTask * findElement(char * cacheresult, int nsize);
            std::list<internalTask *> cache;
    };
}


#endif
