project(myprogs)
cmake_minimum_required(VERSION 2.8)

include_directories("/mnt/c/Users/wangr/Documents/RDMA-EXAMPLE/02")
add_executable(myprog main.cpp)
add_library(rdma SHARED rdma.c)


target_link_libraries(myprog rdma)
target_link_libraries(myprog ibverbs)