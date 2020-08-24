#include <chrono>
#include <iostream>
#include <cstring>
std::chrono::steady_clock::time_point start;
std::chrono::steady_clock::time_point end;
int main(int argc, char* argv[]) {
	int msg_size = 1000000000;
	start = std::chrono::steady_clock::now();
	char* p1 = new char[msg_size];
	end = std::chrono::steady_clock::now();
	std::cout << "memory claim Elapsed time in nanoseconds :"
		<< std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
		<< " ns" << std::endl;
	char* p2 = new char[msg_size];
	memset(p1, 5, msg_size);
	memset(p2, 6, msg_size);
	start = std::chrono::steady_clock::now();
	memcpy(p2, p1, msg_size);
	end = std::chrono::steady_clock::now();
	std::cout << "memory copy Elapsed time in nanoseconds :"
		<< std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
		<< " ns" << std::endl;
}