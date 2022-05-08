#include <gtest/gtest.h>
#include <Taskus.h>
#include <iostream>
#include <atomic>
#include <vector>

TEST(FunctionTaskTest, simpleFunctionTask){
    Taskus::TaskPool tPool;
    tPool.start();
    int i = 0;
    std::vector<int> t = {1,2,2,3,4};
    auto f_task = Taskus::makeFunctionTask<int>([](int i, double t, int r){return int(i*t)*r;}, 3, 3.0, t);
    f_task->changeArgumentsPerTask(2); //should create 3 tasks
    f_task->divideTasks();
    tPool.addTask(f_task->startTask);
    tPool.stop();
    ASSERT_EQ(f_task->result.size(), 5);
    ASSERT_EQ(f_task->result[0], 9);
    ASSERT_EQ(f_task->result[1], 18);
    ASSERT_EQ(f_task->result[2], 18);
    ASSERT_EQ(f_task->result[3], 27);
    ASSERT_EQ(f_task->result[4], 36);
    delete f_task;


}

TEST(FunctionTaskTest, multipleFunctionTasks){
    /*  FunctionTask is designed to simplify the use by creating such class tasks by hand.
        There are some limitations, even though we can put other functionTasks in front of it, we must do it manually 
        (it's very difficult to allow chaining of functiontasks because we can't predict how many they will return beforehand,
        we can simulate this by running the first task, getting the result, creating the second functiontask, running it, and so on.) 
        We can chain a functionTask and then normal tasks, even though it's the user responsability to get the result from the functionTask.

        we can see here a example of chaining multiple function tasks
    */
    Taskus::TaskPool tPool;
    tPool.start();
    //generate a 10 thousand elements
    std::vector<int> t;
    for(int i = 0; i < 10000; i++){
        t.push_back(i);
    }
    std::cout << "Finished filing in the numbers" << "\n";
    //sum the elements a thousand times
    //in this case, this is actually slower than doing nested for loops, because of overhead but this is an example of how how can use it in sequence
    std::vector<Taskus::BaseFunctionClass*> tasks;
    for(int i = 0; i < 10; i++){
        auto f_task = Taskus::makeFunctionTask<int>([](int i){return i+1;}, t);
        f_task->changeArgumentsPerTask(1000);
        f_task->divideTasks();
        tPool.addTask(f_task->startTask);
        f_task->reduceTask->waitToFinish();
        //For now this is returning a vector with size 0 in wsl which is strange, works flawlessly in msvc will test native linux
        t = f_task->result;
        tasks.push_back(f_task);
    }
    tPool.stop();

    for(int i = 0; i < t.size(); i++){
        ASSERT_EQ(i+10, t[i]);
    }    

    for(Taskus::BaseFunctionClass * t: tasks){
        delete t;
    }
}


TEST(FunctionTaskTest, noReturnFunctionTask){
    /*when a function task has return type Taskus::noReturn it means that it is a void function (we can't use the void in templates so this is a workaround)
        it will return nothing it will just execute the function
    */
    Taskus::TaskPool tPool;
    tPool.start();
    std::atomic<int> sum{0};
    std::vector<int> elements;
    for(int i = 0; i <= 100; i++) elements.push_back(i); //create a vector of elements
    //for some reason the compiler doesnt like the function inside the argument
    auto f_task = Taskus::makeFunctionTask<Taskus::noReturn>([&](int i){sum.fetch_add(i);}, elements);
    f_task->changeArgumentsPerTask(10);
    f_task->divideTasks();
    tPool.addTask(f_task->startTask);
    tPool.stop();

    ASSERT_EQ(sum.load(), 5050);
    delete f_task;


}