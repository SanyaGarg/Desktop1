cmake_minimum_required(VERSION 3.10)
project(http_load_test_cpp_update)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_THREAD_LIBS_INIT "-lpthread")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_USE_WIN32_THREADS_INIT 0)
set(CMAKE_USE_PTHREADS_INIT 1)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

add_executable(http_load_test_cpp_update main.cpp)
target_link_libraries(http_load_test_cpp_update ${CURL_LIBRARIES})
