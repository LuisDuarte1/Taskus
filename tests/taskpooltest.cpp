#include <gtest/gtest.h>
#include <Taskus.h>
#include <thread>
#include <chrono>
#include <math.h>




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
    delete t;
    ASSERT_TRUE(stopped);
    
}

//TODO: run example task

class distanceExampleTask : public Taskus::Task{
    public:
        distanceExampleTask(std::vector<float> npoints1, std::vector<float> npoints2) : Task(){
            if((npoints1.size() % 3) != 0 || (npoints2.size() % 3) != 0){
                std::cerr << "Points size are not divisible by three when we are working in three dimensions\n";
                return;
            }
            if(npoints1.size() != npoints2.size()){
                std::cerr << "Points vectors dont have the same size\n";
            }
            points1 = npoints1;
            points2 = npoints2;
        };

        void tryMutate(){

        }
        std::vector<float> result;

        void runTaskFunction(){
            //points are a 3 coordinate each
            for(int i = 0; i < points1.size()/3; i++){
                result.push_back(sqrt(pow((points1[i*3]-points2[i*3]), 2)+
                pow((points1[i*3 + 1]-points2[i*3 + 1]), 2)+
                pow((points1[i*3 + 2]-points2[i*3 + 2]), 2)));
            }
        }
    private:
        std::vector<float> points1;
        std::vector<float> points2;

};


TEST(TaskPoolTest, RunDistanceSimpleTask){
    Taskus::TaskPool * tPool = new Taskus::TaskPool();
    tPool->start();
    std::vector<float> points1 = {
        2,2,1,
        3,4,0,
        9,0,0
    };
    std::vector<float> points2 = {
        0,0,0,
        0,0,0,
        0,0,0
    };
    distanceExampleTask * t = new distanceExampleTask(points1, points2);
    tPool->addTask(t);
    //wait to finish
    t->waitToFinish();

    tPool->stop();

    std::vector<float> result = {
        3,5,9
    };
    ASSERT_EQ(result.size(), t->result.size());

    for(int i = 0; i < result.size(); i++){
        EXPECT_FLOAT_EQ(result[i], t->result[i]) 
        << "Expected: " << result[i] << " and got: " << t->result[i] << " at index: " << i << "\n";
    }
    delete t,tPool;
    
}




TEST(TaskPoolTest, RunDependentTasksTest){
    Taskus::TaskPool * tPool = new Taskus::TaskPool();
    tPool->start();
    std::vector<float> points1 = {
        2,2,1,
        3,4,0,
        9,0,0
    };
    std::vector<float> points2 = {
        0,0,0,
        0,0,0,
        0,0,0
    };
    distanceExampleTask * t = new distanceExampleTask(points1, points2);

}