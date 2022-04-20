#include <gtest/gtest.h>
#include <Taskus.h>
#include <iostream>
#include <atomic>


class sleepDependantTask : public Taskus::Task{
    public:
        sleepDependantTask() : Task(){

        }

        void tryMutate(){
            
        }

        void runTaskFunction(){
            std::cout << "In sleep task\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
};

class simpleBranchTask : public Taskus::BranchTask{
    public:
        simpleBranchTask(Taskus::TaskPool * tPool) : BranchTask(tPool){
        }

        void tryMutate(){

        }

        int chooseBranch(){
            return 1;
        }

        void runTaskFunction(){
            std::cout << "In branch task\n";
        }
};

class boolTask : public Taskus::Task{
    public:
        void tryMutate(){

        }
        void runTaskFunction(){
            std::cout << "Inside branch\n";
            has_run = true;
        } 
        bool has_run  = false;
};

TEST(BranchTaskTest, SimpleBranchTest){
    Taskus::TaskPool * tPool = new Taskus::TaskPool();
    tPool->start();

    sleepDependantTask * st = new sleepDependantTask();
    simpleBranchTask * bt = new simpleBranchTask(tPool);
    *st += bt;
    boolTask * bzero = new boolTask();
    bt->addTaskToBranch(0, bzero);
    boolTask * bone = new boolTask();
    bt->addTaskToBranch(1, bone);
    
    tPool->addTask(st);
    tPool->stop();
    ASSERT_TRUE(bone->has_run);
    ASSERT_FALSE(bzero->has_run);
    delete tPool;
    delete st;
    delete bt;
    delete bzero;
    delete bone;
}

class repeatingBranchTask : public Taskus::BranchTask{
    public:
        repeatingBranchTask(Taskus::TaskPool * tPool) : BranchTask(tPool){
            isRepeatable.store(true);
        }

        std::atomic<int> times_ran{0};

        void tryMutate(){

        }

        int chooseBranch(){
            return times_ran.load() % 2;
        }

        void runTaskFunction(){
            times_ran.store(times_ran.load()+1);
            if(times_ran.load() >= 5){
                isRepeatable.store(false);
            }
        }
};


class decrementTask : public Taskus::Task{
    public:
        decrementTask(int start_n) : Task(){
            n.store(start_n);
        }

        void tryMutate(){}

        void runTaskFunction(){
            n.store(n.load()-1);
        }

        std::atomic<int> n;
};


TEST(BranchTaskTest, RepeatableBranchTask){
    Taskus::TaskPool * tPool = new Taskus::TaskPool();
    tPool->start();
    sleepDependantTask * ts = new sleepDependantTask();
    repeatingBranchTask * bt = new repeatingBranchTask(tPool);
    *ts += bt;
    decrementTask * bzero = new decrementTask(5);
    bt->addTaskToBranch(0, bzero);
    decrementTask * bone = new decrementTask(5);
    bt->addTaskToBranch(1, bone);

    tPool->addTask(ts);
    tPool->stop();
    



    delete tPool;
    delete ts;
    delete bt;
    int resultzero = bzero->n.load();
    int resultone = bone->n.load();
    delete bzero;
    delete bone;
    std::cout << "Branch zero: " << resultzero << "\nBranch one: " << resultone << "\n";
    ASSERT_LE(resultzero, 3);
    ASSERT_LE(resultone, 2);



}
