cmake_minimum_required(VERSION 3.12.0)
project(Taskus VERSION 0.1.0)

#NEEDS GNU G++ VERSION >11
if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 11.0)
    message(FATAL_ERROR "Couldn't find a suitable gcc version > 11.0")
endif()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#use address sanitizer on debug mode
if(UNIX)
  set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
endif(UNIX)
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
  URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.tar.gz
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()


#make a shared library and 
add_library(Taskus SHARED ${SOURCE_FILESS} ${INCLUDE_FILESS})
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_library(atomic-lib atomic)
target_link_libraries(Taskus PRIVATE Threads::Threads atomic)
target_include_directories(Taskus PUBLIC include)
target_compile_features(Taskus PUBLIC cxx_std_20)

install(TARGETS Taskus DESTINATION lib)


include(GoogleTest)

#now, we create the executables for testing
add_executable(tests ${TEST_FILES})
target_link_libraries(tests Threads::Threads Taskus gtest_main)
gtest_discover_tests(tests TEST_LIST testslist)
set_tests_properties(${testslist} PROPERTIES ENVIRONMENT "GUNIT_SHUFFLE=1")
set_tests_properties(${testslist} PROPERTIES TIMEOUT 30)




set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
