#include <gtest/gtest.h>
#include <Taskus.h>


// start and quit threads gracefully capability
TEST(HelloTest, BasicAssertions) {
    std::string t = Taskus::hello_world();
    std::string e = "Hello world!";
    EXPECT_EQ(t, e);
}