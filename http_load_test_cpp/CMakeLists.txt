cmake_minimum_required(VERSION 3.10)
project(http_load_test_cpp)

set(CMAKE_CXX_STANDARD 11)

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

add_executable(http_load_test_cpp main.cpp)
target_link_libraries(http_load_test_cpp ${CURL_LIBRARIES})
