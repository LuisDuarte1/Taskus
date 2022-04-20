#include "internal_tasks/InternalTaskManager.h"

namespace Taskus{
    InternalTaskManager::InternalTaskManager(){

    }
    internalTask* InternalTaskManager::FindInternalTask(internalTask ** t){
        for(auto it = cache.begin(); it != cache.end(); it++){
            if((*t)->cachingFunction() == (*it)->cachingFunction() && (*it)->taskValid.load()){
                return (*it);
            }
        }
        return nullptr;
    }

    bool InternalTaskManager::InsertInternalItem(internalTask ** newItem){
        cacheMutex.lock();
        if(cache.size() == 0){
            cache.push_front(*newItem);
            cacheMutex.unlock();
            return false;
        }
        internalTask * t = FindInternalTask(newItem);
        //if the caching function is the same but the pointer is not the same (to avoid double frees)
        if(t != nullptr && t != (*newItem)){
            delete *newItem;
            *newItem = t;
            cacheMutex.unlock();
            return true;
        }
        if(t == (*newItem)){
            cacheMutex.unlock();
            return true;
        }
        //if not add it to the list if the list has not reached the max size yet
        if(cache.size() < MAX_CACHE_SIZE){
            cache.push_back(*newItem);
            cacheMutex.unlock();
            return false;            
        }
        //the list is full 
        auto it = cache.begin();
        internalTask * toremove = nullptr;

        //so we go from the back and remove the first internalitem not being used no more (which means that the flag taskValid is false)
        while (it != cache.end())
        {
            if((*it)->taskValid.load() == false){
                toremove = *(it);
                break;
            }     
            it++;           

        }
        if(*newItem != toremove && toremove != nullptr){
            delete toremove;
            cache.erase(it);
            cache.push_front(*newItem); 
        }
        cacheMutex.unlock();
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
