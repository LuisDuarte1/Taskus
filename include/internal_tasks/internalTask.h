#ifndef INTERNAL_TASK_H
#define INTERNAL_TASK_H

#include <vector>

#include "../TaskusTask.h"


namespace Taskus{
    class internalTask : public Task{
        public:
            virtual std::vector<char> cachingFunction() = 0;
    };
}


#endif