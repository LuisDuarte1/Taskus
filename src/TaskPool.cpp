#include "TaskusPool.h"

namespace Taskus{


TaskPool::TaskPool(){
    unsigned int n = std::thread::hardware_concurrency(); //get maximum of threads possible
    std::cout << "Number of maximum threads: " << n << "\n";
    for(int i = 0; i < n; i++){
        InterThreadQueue n0;
        InterThreadQueue n1;

        std::array<InterThreadQueue*,2> * nQueues = new std::array<InterThreadQueue*,2>();
        nQueues->at(0) = &n0;
        nQueues->at(1) = &n1;


        TaskusThread * t = new TaskusThread(std::to_string(i),nQueues); 

        threads.emplace_back(t);
        thread_deques.emplace_back(nQueues);
        
    }
}


}
