#include <bits/stdc++.h>
#include "LZW.h"

int main(int argc, char** argv) {
	bool benchmarking = false;
	bool restartDict = false;
	int benchmarkBufferSize = 1;
	long int dictSize = pow(2, sizeof(lzw_code_t)*8)-1;
	if (argc > 0) {
		for (int i = 0; i < argc; i++) {
			if (!strcmp(argv[i], "-b")) {
				benchmarking = true;
			} else if (!strcmp(argv[i], "-m")) {
				if (i+1 < argc)
					benchmarkBufferSize = atoi(argv[i+1]);
				else
					throw std::runtime_error("No benchmark buffer size provided.");
			} else if (!strcmp(argv[i], "--dictSize")) {
				if (i+1 < argc)
					dictSize = std::min((long) atoi(argv[i+1]), dictSize);
				else
					throw std::runtime_error("No dictionary size provided.");
			} else if (!strcmp(argv[i], "-r")) {
				restartDict = true;
			}
		}
	}
	LZW lzw = LZW(benchmarking, dictSize, benchmarkBufferSize, restartDict);

	std::cout << "Compressing: " << argv[1] << "..." << std::endl;
	std::cout << "-> Encoded symbol size: " << sizeof(lzw_code_t)*8 << " bits" << std::endl;
	lzw.compress(argv[1], "compressed.jvav");

	lzw.decompress("compressed.jvav", "decompressed.jvav");

	return 0;
}