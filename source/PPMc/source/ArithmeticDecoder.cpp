#include "ArithmeticDecoder.h"

#define MAX_VALUE 0xFFFFFFFFFFFFFFFFULL
#define HALF (MAX_VALUE / 2 + 1)
#define FIRST_QTR (MAX_VALUE / 4 + 1)
#define THIRD_QTR (3 * FIRST_QTR)


ArithmeticDecoder::ArithmeticDecoder(std::istream& input)
    : input(input), low(0), high(MAX_VALUE), code(0) {
    // Inicializa 'code' com os primeiros bits do arquivo compactado
    for (int i = 0; i < 64; ++i) {
        code = (code << 1) | readBit();
    }

}

uint16_t ArithmeticDecoder::decodeSymbol(const std::vector<std::pair<uint16_t, int>>& frequencies) {
    // Obter a frequência total
    int totalFreq = 0;
    for (const auto& pair : frequencies) {
        totalFreq += pair.second;
    }

    uint64_t range = high - low;
    if (range == 0) {
        std::cerr << "Error: range is zero!" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    // Calcular o valor acumulado (scaled code) dentro do intervalo atual
    uint64_t scaledValue = static_cast<uint64_t>((((code - low) / static_cast<double>(range)) * (totalFreq)));

    // Encontre o símbolo correspondente ao valor acumulado
    uint16_t symbol = 0;
    int cumulativeFreq = 0;
    for (const auto& pair : frequencies) {
        cumulativeFreq += pair.second;
        if (scaledValue < cumulativeFreq) {
            symbol = pair.first;
            break;
        }
    }

    // Obter os limites do símbolo
    int symbolLow = 0;
    int symbolHigh = 0;
    for (const auto& pair : frequencies) {
        if (pair.first == symbol) {
            symbolHigh = symbolLow + pair.second;
            break;
        }
        symbolLow += pair.second;
    }

    // Atualizar os limites do intervalo
    high = low + range * ((1.0*symbolHigh) / (1.0*(totalFreq)));
    low = low + range * ((1.0*symbolLow) / (1.0*totalFreq));

    // Normalizar o intervalo
    normalize();

    return symbol;
}


void ArithmeticDecoder::normalize() {
    while (true) {
        if (high < HALF) {
            // Intervalo está na metade inferior
        } else if (low >= HALF) {
            // Intervalo está na metade superior
            code -= HALF;
            low -= HALF;
            high -= HALF;
        } else if (low >= FIRST_QTR && high < THIRD_QTR) {
            // Intervalo está no meio
            code -= FIRST_QTR;
            low -= FIRST_QTR;
            high -= FIRST_QTR;
        } else {
            break;
        }

        // Dobre os valores de low, high e code
        low <<= 1;
        high = (high << 1) | 1;
        code = (code << 1) | readBit();
    }
}

uint16_t ArithmeticDecoder::readBit() {
    static int bitCount = 0;
    static uint16_t buffer = 0;

    if (bitCount == 0) {
        buffer = input.get();
        bitCount = 8;
    }

    uint16_t bit = (buffer >> (bitCount - 1)) & 1;
    bitCount--;
    return bit;
}
