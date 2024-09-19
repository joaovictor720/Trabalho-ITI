#ifndef ARITHMETIC_DECODER_H
#define ARITHMETIC_DECODER_H

#include <vector>
#include <iostream>
#include <cstdint>
#include <utility>
#include <cmath>

class ArithmeticDecoder {
public:
    ArithmeticDecoder(std::istream& input);
    uint16_t decodeSymbol(const std::vector<std::pair<uint16_t, int>>& frequencies);

private:
    std::istream& input;
    uint64_t low;
    uint64_t high;
    uint64_t code;
    
    uint16_t readBit();
    void normalize();
};

#endif
