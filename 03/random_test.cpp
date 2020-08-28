#include <chrono>
#include <iostream>
#include <cstring>
std::chrono::steady_clock::time_point start;
std::chrono::steady_clock::time_point end;
int main(int argc, char* argv[]) {
	int msg_size = 4;
	start = std::chrono::steady_clock::now();
	char* p1 = new char[msg_size];
	end = std::chrono::steady_clock::now();
	std::cout << "memory claim Elapsed time in nanoseconds :"
		<< std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
		<< " ns" << std::endl;
	char* p2 = new char[msg_size];
	
	
	if (msg_size < 32 * 1024 * 1024) {
		memset(p1, 5, 1000000000);
		memset(p2, 6, 1000000000);
		start = std::chrono::steady_clock::now();
		for (int i = 0; i < 20; i++) {
			memcpy(p2 + 16 * 1024 * 1024 * i, p1 + 16 * 1024 * 1024 * i, msg_size);
		}
	}
	else {
		memset(p1, 5, msg_size);
		memset(p2, 6, msg_size);
		start = std::chrono::steady_clock::now();
		
		for (int i = 0; i < 20; i++) {
			memcpy(p2, p1, msg_size);
		}
	}
	
	
	end = std::chrono::steady_clock::now();
	std::cout << "memory copy Elapsed time in nanoseconds :"
		<< std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()/20.0
		<< " ns" << std::endl;
}