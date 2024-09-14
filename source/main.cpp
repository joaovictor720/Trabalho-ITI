#include <bits/stdc++.h>
#include "LZW.h"

int main(int argc, char** argv) {
	LZW lzw = LZW();

	if (argc > 3) {
		if (!strcmp(argv[3], "-c")) {
			lzw.compress(argv[1], argv[2]);
		} else if (!strcmp(argv[3], "-d")) {
			lzw.decompress(argv[1], argv[2]);
		}
		return 0;
	}

	lzw.compress(argv[1], "compressed.jvav");
	lzw.decompress("compressed.jvav", "decompressed.jvav");

	return 0;
}