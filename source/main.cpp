#include <bits/stdc++.h>
#include "LZW.h"

int main(int argc, char** argv) {
	LZW lzw = LZW();
	
	lzw.compress(argv[1], argv[2]);

	return 0;
}