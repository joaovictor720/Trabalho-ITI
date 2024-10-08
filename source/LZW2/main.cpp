#include <bits/stdc++.h>
#include "LZW2.h"

int main(int argc, char** argv) {
	bool restart_map_on_overflow = false;
	bool compress = false;
	bool decompress = false;
	long int max_map_phrases = (1 << sizeof(lzw_code_t)*8) - 1;
	std::string input;
	std::string output;

	if (argc > 0) {
		for (int i = 0; i < argc; i++) {
			if (!strcmp("--max", argv[i])) {
				if (i+1 < argc) {
					max_map_phrases = atoi(argv[i+1]);
				}
			} else if (!strcmp("-r", argv[i])) {
				restart_map_on_overflow = true;
			} else if (!strcmp("-c", argv[i])) {
				compress = true;
				if (i+2 < argc) {
					input = argv[i+1];
					output = argv[i+2];
				}
			} else if (!strcmp("-d", argv[i])) {
				decompress = true;
				if (i+2 < argc) {
					input = argv[i+1];
					output = argv[i+2];
				}
			}
		}
	}
	
	LZW2 lzw2;
	lzw2.set_max_phrases(max_map_phrases);
	lzw2.set_restart_map_on_overflow(restart_map_on_overflow);

	if (compress) {
		lzw2.compress(input, output);
	} else if (decompress) {
		lzw2.decompress(input, output);
	} else {
		std::cout << "Num vai nem comprimir nem descomprimir, vai fazer oq??";
	}
	
	return 0;
}