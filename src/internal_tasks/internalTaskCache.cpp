#include "internal_tasks/internalTaskCache.h"

namespace Taskus{
    InternalTaskCache::InternalTaskCache(){

    }
    bool InternalTaskCache::InsertInternalItem(internalTask ** newItem){
        if(cache.size() == 0){
            cache.push_front(*newItem);
            return false;
        }
        int nsize = 0;
        char * newItemcache = (*newItem)->cachingFunction(&nsize);
        if(cache.size() < MAX_CACHE_SIZE){
            //find element
            internalTask * foundTask = findElement(newItemcache, nsize);
            delete[] newItemcache;
            //if it found, erase element and put it at the front
            //also remove newTask because it's not going to be used
            if(foundTask != nullptr){
                if((*newItem) != foundTask){
                    delete (*newItem);
                    *newItem = foundTask;
                }

                cache.remove(foundTask);
                cache.push_front(foundTask);
                //true for cache hit
                return true;
            } 
            //if it not found simply insert it
            else{
                cache.push_front(*newItem);
                //cache miss
                return false;
            }
        }
        internalTask * foundTask = findElement(newItemcache, nsize);
        delete[] newItemcache;
        if(foundTask == nullptr){
            //delete last element
            delete *(--cache.end());
            cache.pop_back();
            cache.push_front(*newItem);
            //cache miss
            return false;
        }
        delete *newItem;
        *newItem = foundTask;
        cache.remove(foundTask);
        cache.push_front(foundTask);
        return true;

    }


    InternalTaskCache::~InternalTaskCache(){
        for(int i = 0; i < cache.size(); i++){
            auto it = cache.begin();
            std::advance(it, i);
            delete *it;
        }
        cache.clear();
    }

    internalTask * InternalTaskCache::findElement(char * cacheresult, int nsize){
        //find element
        internalTask * foundTask = nullptr;
        for(auto it = cache.begin(); it != cache.end(); it++){
            int msize = 0;
            char * itcache = (*it)->cachingFunction(&msize);
            if(msize == nsize){
                //compare cache result if size is equal
                bool equal = true;
                for(int e = 0; e < msize; e++){
                    if(itcache[e] != cacheresult[e]){
                        equal = false;
                        break;
                    }
                }
                if(equal){
                    foundTask = *it;
                    delete[] itcache;
                    break;
                }
                delete[] itcache;

            }

        }

        return foundTask;
    }
}