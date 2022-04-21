cmake_minimum_required(VERSION 3.12.0)
project(Taskus VERSION 0.1.0)

#PROJECT OPTIONS
unset(TASKUS_BUILD_TESTS CACHE)
unset(TASKUS_BUILD_BENCHMARKS CACHE)

set(TASKUS_BUILD_TESTS ON)
set(TASKUS_BUILD_BENCHMARKS ON)


#NEEDS GNU G++ VERSION >11
if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 11.0)
    message(FATAL_ERROR "Couldn't find a suitable gcc version > 11.0")
endif()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(UNIX)
  #use address sanitizer on debug mode
  set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")

  #force using mold in linux, it's wayy faster
  find_program(MOLD_EXISTS "mold")
  if(MOLD_EXISTS)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
      set(CMAKE_CXX_FLAGS "-fuse-ld=mold ${CMAKE_CXX_FLAGS}")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      #this assumes a correct instalation of mold
      #only gcc 12.1.0 onwards (which is a future release for the moment) will suport -fuse-ld=mold
      set(CMAKE_CXX_FLAGS "-B/usr/local/libexec/mold ${CMAKE_CXX_FLAGS}")
    else()
      message(FATAL_ERROR "Using a unsupported compiler for mold.")
    endif()


  else()
    message(FATAL_ERROR "Couldn't find mold linker.")
  endif(MOLD_EXISTS)
  
endif(UNIX)
#get source files
#it's not optimal to get the sources files using glob recurse but it's the best way I think in this case
#if anything, we have to delete build and rebuild it 
file(GLOB_RECURSE SOURCE_FILESS "src/*.h" "src/*.cpp")
file(GLOB_RECURSE INCLUDE_FILESS "include/*.h")

#make a shared library and 
add_library(Taskus SHARED ${SOURCE_FILESS} ${INCLUDE_FILESS})
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_library(atomic-lib atomic)
target_link_libraries(Taskus PRIVATE Threads::Threads atomic)
target_include_directories(Taskus PUBLIC include)
target_compile_features(Taskus PUBLIC cxx_std_20)

install(TARGETS Taskus DESTINATION lib)

if(TASKUS_BUILD_TESTS)
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





  include(GoogleTest)

  #now, we create the executables for testing
  add_executable(tests ${TEST_FILES})
  target_link_libraries(tests Threads::Threads Taskus gtest_main)
  gtest_discover_tests(tests TEST_LIST testslist)
  set_tests_properties(${testslist} PROPERTIES ENVIRONMENT "GUNIT_SHUFFLE=1")
  set_tests_properties(${testslist} PROPERTIES TIMEOUT 30)

endif(TASKUS_BUILD_TESTS)


if(TASKUS_BUILD_BENCHMARKS)
  file(GLOB_RECURSE BENCH_FILES "bench/*.cpp")
  foreach(file ${BENCH_FILES})
    cmake_path(GET file STEM filename)    
    add_executable(${filename} ${file})
    target_link_libraries(${filename} Taskus)
    set_target_properties(${filename} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bench")

  endforeach(file ${BENCH_FILES})
  
endif(TASKUS_BUILD_BENCHMARKS)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
