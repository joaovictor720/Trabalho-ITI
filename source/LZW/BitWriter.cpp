#include "BitWriter.h"

// BitWriter::BitWriter(const std::string &filename) : bitBuffer(0), bitCount(0) {
// 	outputFile.open(filename, std::ios::binary);
// }

void BitWriter::writeBits(lzw_code_t code, int bitWidth) {
	for (int i = bitWidth - 1; i >= 0; --i) {
		bool bit = (code >> i) & 1;
		bitBuffer.push_back(bit);
		currentBitPosition++;
	}
}

void BitWriter::flushToFile(std::ofstream& output) {
	uint8_t byte = 0;
	for (size_t i = 0; i < bitBuffer.size(); ++i) {
		byte |= (bitBuffer[i] << (7 - (i % 8)));
		if ((i + 1) % 8 == 0) {
			output.put(byte);
			byte = 0;
		}
	}

	// Flush remaining bits that don't make a full byte
	if (bitBuffer.size() % 8 != 0) {
		output.put(byte);
	}

	// Clear the buffer once flushed
	bitBuffer.clear();
}

void BitWriter::addCode(lzw_code_t code, int bitWidth) {
	writeBits(code, bitWidth);
}

void BitWriter::writeToFile(const std::string& filename) {
	std::ofstream output(filename, std::ios::binary);
	if (!output.is_open()) {
		throw std::runtime_error("Unable to open output file");
	}
	flushToFile(output);
	output.close();
}

// #include <bitset>

// // Writes code with the current bit width to the buffer
// void BitWriter::writeCode(lzw_code_t code, int bitWidth) {
// 	std::bitset<24> temp = code;
// 	// std::cout << "trying to write " << bitWidth << " bits from " << temp << " = " << temp.to_ulong() << std::endl;
// 	// Add the code to the bit buffer
// 	bitBuffer = (bitBuffer << bitWidth) | code;
// 	bitCount += bitWidth;

// 	// Write out full bytes if possible
// 	while (bitCount >= 8) {
// 		// Write the most significant byte from the buffer
// 		uint8_t byteToWrite = (bitBuffer >> (bitCount - 8)) & 0xFF;
// 		// outputFile.put(byteToWrite);
// 		std::bitset<8> temp2 = byteToWrite;
// 		// std::cout << "writing " << temp2 << " = " << temp2.to_ulong() << std::endl;
// 		// temp = bitBuffer;
// 		// std::cout << "buffer " << temp << std::endl;
// 		outputFile.write(reinterpret_cast<const char*>(&byteToWrite), sizeof(byteToWrite));
// 		bitCount -= 8;
// 	}
// }

// // Flushes any remaining bits (padded to full bytes)
// void BitWriter::flush() {
// 	if (bitCount > 0) {
// 		uint8_t byteToWrite = (bitBuffer << (8 - bitCount)) & 0xFF;
// 		std::bitset<8> temp = byteToWrite;
// 		// std::cout << "Flushing " << temp << " = " << temp.to_ulong() << std::endl;
// 		outputFile.write(reinterpret_cast<const char*>(&byteToWrite), sizeof(byteToWrite));
// 		// outputFile.put(byteToWrite);
// 	}
// 	outputFile.close();
// }