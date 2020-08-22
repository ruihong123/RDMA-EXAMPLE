project(myprogs)
cmake_minimum_required(VERSION 2.8)

include_directories("/mnt/c/Users/wangr/Documents/RDMA-EXAMPLE/02")
add_executable(myprog main.cpp)
add_library(rdma SHARED "rdma.cpp")
target_link_libraries(rdma ibverbs)

target_link_libraries(myprog rdma)
