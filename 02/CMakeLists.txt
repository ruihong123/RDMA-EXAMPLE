project(myprogs)
cmake_minimum_required(VERSION 2.8)
SET(CMAKE_CXX_FLAGS "-std=c++0x")
include_directories($(PROJECT_SOURCE_DIR))
add_executable(myprog main.cpp)
add_library(rdma SHARED "rdma.cpp")
target_link_libraries(rdma ibverbs)

target_link_libraries(myprog rdma)
