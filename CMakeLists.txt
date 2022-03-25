cmake_minimum_required(VERSION 3.12.0)
project(Taskus VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)

#get source files
#it's not optimal to get the sources files using glob recurse but it's the best way I think in this case
#if anything, we have to delete build and rebuild it 
file(GLOB_RECURSE SOURCE_FILESS "src/*.h" "src/*.cpp")
file(GLOB_RECURSE INCLUDE_FILESS "include/*.h")


file(GLOB_RECURSE TEST_FILES "tests/*.cpp")


#get test_files

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()


#make a shared library and 
add_library(Taskus SHARED ${SOURCE_FILESS} ${INCLUDE_FILESS})
target_include_directories(Taskus PUBLIC include)
target_compile_features(Taskus PUBLIC cxx_std_20)

install(TARGETS Taskus DESTINATION lib)


include(GoogleTest)

#now, we create the executables for testing
add_executable(tests ${TEST_FILES})
target_link_libraries(tests Taskus gtest_main)
gtest_discover_tests(tests)



set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
