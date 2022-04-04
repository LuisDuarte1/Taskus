#include <gtest/gtest.h>
#include <Taskus.h>
#include <thread>
#include <chrono>



// test: start and quit threads gracefully capability
TEST(TaskPoolTest, StartAndStopTest) {
    Taskus::TaskPool * t = new Taskus::TaskPool();
    t->start();
    //create a thread to do the timeout function
    std::chrono::time_point before_running = std::chrono::system_clock::now();
    std::thread timeoutFunction([t](){
        t->stop();
    });
    bool stopped = false;
    //check for a max of 5 seconds that the thread is joinable aka: it has finished execution
    while(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - before_running).count() < 5){
        if(timeoutFunction.joinable()){
            stopped = true;
            timeoutFunction.join();
            break;
        }
    }
    ASSERT_TRUE(stopped);
    
}


//TODO: run example task