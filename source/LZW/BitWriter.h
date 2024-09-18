#ifndef BITWRITER_H
#define BITWRITER_H

#pragma once

#include <fstream>
#include <vector>
#include <cstdint>

class BitWriter {
private:
    std::ofstream outputFile;
    uint64_t bitBuffer; // Accumulates bits
    int bitCount;       // Tracks number of bits in the buffer
    
public:
    BitWriter(const std::string &filename);
    ~BitWriter();

    // Writes code with the current bit width to the buffer
    void writeCode(uint16_t code, int bitWidth);

    // Flushes any remaining bits (padded to full bytes)
    void flush();
};


#endif