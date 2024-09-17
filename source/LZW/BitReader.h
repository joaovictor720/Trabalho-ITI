#ifndef BITREADER_H
#define BITREADER_H

#pragma once

#include <fstream>
#include <vector>
#include <cstdint>

class BitReader {
private:
    std::ifstream inputFile;
    uint64_t bitBuffer; // Stores bits from the file
    int bitCount;       // Tracks the number of bits currently in the buffer
    
public:
    BitReader(const std::string &filename);
    ~BitReader();

    // Reads 'bitWidth' bits from the input stream and returns them as a code
    uint16_t readCode(int bitWidth);

    // Closes the input file
    void close();
};

#endif