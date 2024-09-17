#include "BitReader.h"

BitReader::BitReader(const std::string &filename) : bitBuffer(0), bitCount(0) {
	inputFile.open(filename, std::ios::binary);
}

BitReader::~BitReader()
{

}

uint16_t BitReader::readCode(int bitWidth) {
	// Ensure we have enough bits in the buffer
	while (bitCount < bitWidth) {
		char byte;
		if (!inputFile.get(byte)) {
			// If we run out of data, return some sort of error or handle EOF
			throw std::runtime_error("Unexpected end of file.");
		}
		bitBuffer = (bitBuffer << 8) | (uint8_t)byte;
		bitCount += 8;
	}

	// Extract the required number of bits
	uint16_t code = (bitBuffer >> (bitCount - bitWidth)) & ((1 << bitWidth) - 1);
	bitCount -= bitWidth;

	return code;
}

void BitReader::close()  {
	inputFile.close();
}