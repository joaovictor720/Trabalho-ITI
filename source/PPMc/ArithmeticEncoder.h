#ifndef ARITHMETIC_ENCODER_H
#define ARITHMETIC_ENCODER_H

#include <iostream>
#include <vector>
#include <utility> // Para std::pair
#include <cstdint>

class ArithmeticEncoder {
public:
    ArithmeticEncoder(std::ostream& output);
    ~ArithmeticEncoder();

    void encodeSymbol(uint16_t symbol, const std::vector<std::pair<uint16_t, int>>& frequencies);
    void finishEncoding();

private:
    std::ostream& output;

    uint64_t low;
    uint64_t high;
    int oppositeBits;
    uint16_t buffer;
    int bitsInBuffer;

    void outputBits(int bit);
    void writeBit(int bit);
    void normalize();
};

#endif // ARITHMETIC_ENCODER_H
