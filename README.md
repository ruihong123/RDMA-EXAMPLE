# RDMA-EXAMPLE
RDMA学习
# 02 RDMA test
cd 02
cd build
cmake ..
make 
./myprog -s 1024
-s denoting the size of the RDMA message
# 03 Memory copy test
g++ -o out random_test.cpp get_clock.c
./out
