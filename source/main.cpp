#include <bits/stdc++.h>
#include "LZW/LZW.h"

int main(int argc, char** argv) {
	bool benchmarking = false;
	bool restartDict = false;
	int benchmarkBufferSize = 1;
	bool training = false;
	std::string trainedModel;
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
			} else if (!strcmp(argv[i], "-d")) {
				if (i+1 < argc)
					dictSize = std::min((long) atoi(argv[i+1]), dictSize);
				else
					throw std::runtime_error("No dictionary size provided.");
			} else if (!strcmp(argv[i], "-r")) {
				restartDict = true;
			} else if (!strcmp(argv[i], "--train")) {
				training = true;
			} else if (!strcmp(argv[i], "--use")) {
				if (i+1 < argc)
					trainedModel = argv[i+1];
				else
					throw std::runtime_error("No model path provided.");
			}
		}
	}

	// std::vector<uint8_t> test;
	// for (int i = 0; i < 4; i++) {
	// 	test.push_back((uint8_t) 0xff);
	// }
	// std::ofstream file("file");
	// file.write(reinterpret_cast<const char*>(test.data()), test.size());
	// file.close();

	LZW lzw = LZW(benchmarking, dictSize, benchmarkBufferSize, restartDict, training);

	if (trainedModel.size())
		lzw.usingModel(trainedModel);

	std::cout << "Compressing: " << argv[1] << "..." << std::endl;
	lzw.compress(argv[1], "compressed.jvav");

	std::cout << "\nDecompressing: " << "compressed.jvav" << "..." << std::endl;
	lzw.decompress("compressed.jvav", "decompressed");

	// ler arquivo descomprimido
	// std::ifstream decomp("decompressed");
	// char byte;
	// while (!decomp.eof()) {
	// 	byte = decomp.get();
	// 	std::bitset<8> temp = byte;
	// 	std::cout << temp << " = " << temp.to_ulong() << std::endl;
	// }

	return 0;
}