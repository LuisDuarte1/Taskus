#include "internal_tasks/InternalTaskManager.h"

namespace Taskus{
    InternalTaskManager::InternalTaskManager(){

    }
    int InternalTaskManager::FindInternalTask(internalTask ** t){
        int i = 0;
        bool found = false;
        for(auto it = cache.begin(); it != cache.end(); it++){
            if(*it == *t) {
                found = true;
                break;
            }
            i++;
        }
        if(!found) return -1;
        return i;
    }

    bool InternalTaskManager::InsertInternalItem(internalTask ** newItem){
        if(cache.size() == 0){
            cache.push_front(*newItem);
            return false;
        }
        if(cache.size() < MAX_CACHE_SIZE){
            if(FindInternalTask(newItem) == -1)cache.push_back(*newItem);
            return false;
        }
        if(FindInternalTask(newItem) == -1){
            internalTask * toremove = *(cache.rbegin());
            delete toremove;
            cache.pop_back();
            cache.push_front(*newItem);
            return true;
        }
        return false;

    }


    InternalTaskManager::~InternalTaskManager(){
        std::vector<internalTask*> toDeleteVec;
        for(auto it = cache.begin(); it != cache.end(); it++){
            internalTask * toDelete = *it;
            toDeleteVec.push_back(toDelete);
        }
        cache.clear();
        for(int i = 0; i < toDeleteVec.size(); i++){
            delete toDeleteVec[i];
        }
    }

}