#ifndef INTERNAL_TASK_H
#define INTERNAL_TASK_H


#include "../TaskusTask.h"

namespace Taskus{
    class internalTask : public Task{
        public:
            //this will allocate memory for the return of the caching function, that means that it will
            //need to be deleted, to avoid memory leaks
            virtual char * cachingFunction(int * n) = 0;
    };
}


#endif