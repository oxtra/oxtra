#include <iostream>
#include <inttypes.h>
using namespace std;

int main() {
	// initialize the attributes
	uint8_t* ptr = reinterpret_cast<uint8_t*>(0x400000);
	uint64_t size = 0x1C3000;

	// print this image
	for(auto i = 0; i < size; i++) {
		if(reinterpret_cast<uintptr_t>(ptr) < 0x5b1000 || reinterpret_cast<uintptr_t>(ptr) >= 0x5b2000)
			cout << *ptr++;
		else {
			cout << (uint8_t)0;
			ptr++;
		}
	}

	return 0;
}