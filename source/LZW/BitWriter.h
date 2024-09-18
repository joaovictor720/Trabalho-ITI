#ifndef BITWRITER_H
#define BITWRITER_H

#pragma once

#include <fstream>
#include <vector>
#include <cstdint>
typedef uint32_t lzw_code_t;

class BitWriter {
private:
    std::vector<bool> bitBuffer;
    size_t currentBitPosition = 0;

    // Write individual bits from the code to the buffer
    void writeBits(lzw_code_t code, int bitWidth);

    // Helper to flush the bits in the buffer to a file
    void flushToFile(std::ofstream& output);

public:
    // Function to add a code to the bit buffer with a given bit width
    void addCode(lzw_code_t code, int bitWidth);

    // Function to write the buffer to a file
    void writeToFile(const std::string& filename);
};


#endif