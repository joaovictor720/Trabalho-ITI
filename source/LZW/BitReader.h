#ifndef BITREADER_H
#define BITREADER_H

#pragma once

#include <fstream>
#include <vector>
#include <cstdint>
#include "BitWriter.h"

class BitReader {
private:
    std::vector<bool> bitBuffer;
    size_t currentBitPosition = 0;

    // Helper to read bytes from a file and fill the bit buffer
    void loadFromFile(const std::string& filename);

    // Extract a code of the specified bit width from the buffer
    lzw_code_t readBits(int bitWidth);

public:
    // Load the bits from the file into the buffer
    void readFromFile(const std::string& filename);

    // Function to retrieve the next code of the given bit width
    lzw_code_t getNextCode(int bitWidth);
};

#endif