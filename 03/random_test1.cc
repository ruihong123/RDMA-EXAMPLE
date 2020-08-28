extern "C" {
#include <get_clock.h>
}
#include <iostream>
#include <cstring>
//std::chrono::steady_clock::time_point start;
//std::chrono::steady_clock::time_point end;
int main(int argc, char* argv[]) {

	double cycles_to_units = get_cpu_mhz(0) * 1000000;

	double format_factor = 0x100000;// : 125000000;

	int msg_size = 1000;
	char* p1 = new char[msg_size];
	char* p2 = new char[msg_size];
	memset(p1, 5, msg_size);
	memset(p2, 6, msg_size);
	cycles_t start = get_cycles();
	for (int i = 0; i < 1000; i++) {

		memcpy(p2, p1, msg_size);
	}
	
	cycles_t end = get_cycles();
	double bw_avg = end - start;//((double)(msg_size)*cycles_to_units) / ((end - start) * format_factor);
	std::cout << "memory copy Elapsed time  :"
		<< bw_avg/1000
		<< " cycles" << std::endl;
}