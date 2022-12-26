cmake_minimum_required(VERSION 3.1)


project(testperf)

set(CMAKE_CXX_STANDARD 11)

find_path(GPERFTOOLS_INCLUDE_DIR NAMES gperftools/heap-profiler.h)
find_library(GPERFTOOLS_LIBRARIES NAMES tcmalloc_and_profiler)
include_directories(${GPERFTOOLS_INCLUDE_DIR})



add_executable(testCpu testCpu.cpp)
target_link_libraries(testCpu ${GPERFTOOLS_LIBRARIES})


add_executable(testheap testHeap.cpp)
target_link_libraries(testheap ${GPERFTOOLS_LIBRARIES} pthread -unwind)
