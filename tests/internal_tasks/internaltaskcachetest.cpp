#include <gtest/gtest.h>
#include <Taskus.h>

class test_dependencies_task : public Taskus::Task{
        public:
            void runTaskFunction(){           
                return;
            }
            void tryMutate(){
                return;
            }

};

TEST(InternalTaskCacheTest, MissFirstElementTest){
    Taskus::InternalTaskCache *tcache = new Taskus::InternalTaskCache();

    test_dependencies_task * task = new test_dependencies_task();
    Taskus::internalRepeatTask * t = new Taskus::internalRepeatTask(task, nullptr);
    Taskus::internalTask * tt = t;

    
    bool cache_result = tcache->InsertInternalItem(&tt);
    ASSERT_FALSE(cache_result);

    delete tcache;
    delete task;

}

TEST(InternalTaskCacheTest, HitFirstElementTest){
    Taskus::InternalTaskCache * tcache = new Taskus::InternalTaskCache();

    test_dependencies_task * task = new test_dependencies_task();
    Taskus::internalRepeatTask * t = new Taskus::internalRepeatTask(task, nullptr);
    
    Taskus::internalTask * tt = t;

    bool cache_result = tcache->InsertInternalItem(&tt);
    cache_result = tcache->InsertInternalItem(&tt);

    ASSERT_TRUE(cache_result);

    delete tcache;
    delete task;
}

TEST(InternalTaskCacheTest, HitSomeElementTest){
    Taskus::InternalTaskCache * tcache = new Taskus::InternalTaskCache();

    test_dependencies_task ** tasks = new test_dependencies_task*[5];
    Taskus::internalRepeatTask ** ts = new Taskus::internalRepeatTask*[5];
    for(int i = 0; i < 5; i++){
        tasks[i] = new test_dependencies_task();

        ts[i] = new Taskus::internalRepeatTask(tasks[i], nullptr);
        Taskus::internalTask * tt = ts[i];

        bool cache_result = tcache->InsertInternalItem(&tt);
        ASSERT_FALSE(cache_result);
    }

    Taskus::internalRepeatTask * repeatedt = new Taskus::internalRepeatTask(tasks[3], nullptr);
    Taskus::internalTask * tt = repeatedt;
    bool crepeated = tcache->InsertInternalItem(&tt);
    ASSERT_TRUE(crepeated);
    

    for(int i = 0; i < 5; i++){
        delete tasks[i];
    }
    delete tcache;
    delete[] tasks;
    delete[] ts;
}


TEST(InternalTaskCacheTest, HitFullCacheElementTest){
    Taskus::InternalTaskCache * tcache = new Taskus::InternalTaskCache();

    test_dependencies_task ** tasks = new test_dependencies_task*[MAX_CACHE_SIZE];
    Taskus::internalRepeatTask ** ts = new Taskus::internalRepeatTask*[MAX_CACHE_SIZE];
    for(int i = 0; i < MAX_CACHE_SIZE; i++){
        tasks[i] = new test_dependencies_task();
        ts[i] = new Taskus::internalRepeatTask(tasks[i], nullptr);
        Taskus::internalTask * tt = ts[i];

        bool cache_result = tcache->InsertInternalItem(&tt);
        ASSERT_FALSE(cache_result);
    }

    Taskus::internalRepeatTask * repeatedt = new Taskus::internalRepeatTask(tasks[3], nullptr);
    Taskus::internalTask * tt = repeatedt;
    bool crepeated = tcache->InsertInternalItem(&tt);
    ASSERT_TRUE(crepeated);
    

    for(int i = 0; i < MAX_CACHE_SIZE; i++){
        delete tasks[i];
    }
    delete tcache;
    delete[] tasks;
    delete[] ts;
}

TEST(InternalTaskCacheTest, MissFullCacheElementTest){
    Taskus::InternalTaskCache * tcache = new Taskus::InternalTaskCache();

    test_dependencies_task ** tasks = new test_dependencies_task*[MAX_CACHE_SIZE];
    Taskus::internalRepeatTask ** ts = new Taskus::internalRepeatTask*[MAX_CACHE_SIZE];
    for(int i = 0; i < MAX_CACHE_SIZE; i++){
        tasks[i] = new test_dependencies_task();
        ts[i] = new Taskus::internalRepeatTask(tasks[i], nullptr);
        Taskus::internalTask * tt = ts[i];

        bool cache_result = tcache->InsertInternalItem(&tt);
        ASSERT_FALSE(cache_result);
    }

    test_dependencies_task * misst = new test_dependencies_task();
    Taskus::internalRepeatTask * repeatedt = new Taskus::internalRepeatTask(misst, nullptr);
    Taskus::internalTask * tt = repeatedt;

    bool crepeated = tcache->InsertInternalItem(&tt);
    ASSERT_FALSE(crepeated);
    

    for(int i = 0; i < MAX_CACHE_SIZE; i++){
        delete tasks[i];
    }
    delete misst;
    delete tcache;
    delete[] tasks;
    delete[] ts;
}