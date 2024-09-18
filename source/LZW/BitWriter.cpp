#include "BitWriter.h"

BitWriter::BitWriter(const std::string &filename) : bitBuffer(0), bitCount(0) {
	outputFile.open(filename, std::ios::binary);
}

BitWriter::~BitWriter()
{

}

// Writes code with the current bit width to the buffer
void BitWriter::writeCode(uint16_t code, int bitWidth) {
	// Add the code to the bit buffer
	bitBuffer = (bitBuffer << bitWidth) | code;
	bitCount += bitWidth;

	// Write out full bytes if possible
	while (bitCount >= 8) {
		// Write the most significant byte from the buffer
		uint8_t byteToWrite = (bitBuffer >> (bitCount - 8)) & 0xFF;
		outputFile.put(byteToWrite);
		bitCount -= 8;
	}
}

// Flushes any remaining bits (padded to full bytes)
void BitWriter::flush() {
	if (bitCount > 0) {
		uint8_t byteToWrite = (bitBuffer << (8 - bitCount)) & 0xFF;
		outputFile.put(byteToWrite);
	}
	outputFile.close();
}