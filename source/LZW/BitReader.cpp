#include "BitReader.h"

// BitReader::BitReader(const std::string &filename) : bitBuffer(0), bitCount(0) {
// 	inputFile.open(filename, std::ios::binary);
// }

void BitReader::loadFromFile(const std::string& filename) {
	std::ifstream input(filename, std::ios::binary);
	if (!input.is_open()) {
		throw std::runtime_error("Unable to open input file");
	}

	char byte;
	while (input.get(byte)) {
		for (int i = 7; i >= 0; --i) {
			bool bit = (byte >> i) & 1;
			bitBuffer.push_back(bit);
		}
	}

	input.close();
}

lzw_code_t BitReader::readBits(int bitWidth) {
	lzw_code_t code = 0;
	for (int i = 0; i < bitWidth; ++i) {
		code <<= 1;
		code |= bitBuffer[currentBitPosition];
		currentBitPosition++;
	}
	return code;
}

void BitReader::readFromFile(const std::string& filename) {
	loadFromFile(filename);
}

lzw_code_t BitReader::getNextCode(int bitWidth) {
	if (currentBitPosition + bitWidth > bitBuffer.size()) {
		throw std::runtime_error("Not enough bits in the buffer");
	}
	return readBits(bitWidth);
}

// #include <bitset>

// lzw_code_t BitReader::readCode(const int bitWidth) {

// 	// Ler enquanto o próximo código ainda não tiver sido lido completamente
// 	while (bitCount < bitWidth) {
// 		char byte;

// 		// Forma de ler o arquivo até EOF
// 		if (!inputFile.read(&byte, 1)) {
// 			throw std::runtime_error("Unexpected end of file.");
// 		}
// 		bitBuffer = (bitBuffer << 8) | (uint8_t) byte;
// 		bitCount += 8;
// 	}

// 	// Extraindo os bits do código, de acordo com o tamanho atual do código
// 	lzw_code_t code = (bitBuffer >> (bitCount - bitWidth)) & ((1 << bitWidth) - 1);
// 	bitCount -= bitWidth;

// 	return code;
// }

// void BitReader::close()  {
// 	inputFile.close();
// }
