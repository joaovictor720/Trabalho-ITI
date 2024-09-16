#include "ArithmeticEncoder.h"

#define MAX_VALUE 0xFFFFFFFFFFFFFFFFULL
#define HALF (MAX_VALUE / 2 + 1)
#define FIRST_QTR (MAX_VALUE / 4 + 1)
#define THIRD_QTR (3 * FIRST_QTR)

ArithmeticEncoder::ArithmeticEncoder(std::ostream& output)
    : output(output), low(0), high(MAX_VALUE), oppositeBits(0), buffer(0), bitsInBuffer(0) {}

ArithmeticEncoder::~ArithmeticEncoder() {
    finishEncoding();  // Certificar que chamamos finishEncoding no destrutor também.
}

void ArithmeticEncoder::encodeSymbol(uint16_t symbol, const std::vector<std::pair<uint16_t, int>>& frequencies) {
    // Calcular a frequência total
    int totalFreq = 0;
    for (const auto& pair : frequencies) {
        totalFreq += pair.second;
    }

/*     std::cout << "------------------------ Codificando:" << symbol << std::endl; */

    // Calcular o intervalo
    uint64_t range = high - low;

/*     // Depuração: Imprimir low, high e range
    std::cout << "Antes da atualização:" << std::endl;
    std::cout << "low: " << low << std::endl;
    std::cout << "high: " << high << std::endl;
    std::cout << "range: " << range << std::endl; */

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

/*     // Depuração: Imprimir symbolLow e symbolHigh
    std::cout << "symbolLow: " << symbolLow << std::endl;
    std::cout << "symbolHigh: " << symbolHigh << std::endl;
    std::cout << "totalFreq: " << totalFreq << std::endl; */

    // Verificar se symbolLow e symbolHigh são válidos
    if (symbolHigh <= symbolLow || totalFreq == 0) {
        std::cerr << "Erro: Intervalo inválido para o símbolo." << std::endl;
        return;
    }

    // Atualizar os limites low e high
    high = low + range * (1.0*(symbolHigh) / (1.0*(totalFreq)));
    low = low + range * ((1.0*symbolLow) / (1.0*totalFreq));

/*     // Depuração: Imprimir low e high após a atualização
    std::cout << "Após a atualização:" << std::endl;
    std::cout << "low: " << low << std::endl;
    std::cout << "high: " << high << std::endl; */

    // Normalizar o intervalo
    normalize();

    // Depuração: Imprimir low e high após a atualização
    //std::cout << "Após a atualização:" << std::endl;
    //std::cout << "low: " << low << std::endl;
    //std::cout << "high: " << high << std::endl;

}


void ArithmeticEncoder::normalize() {
    // Normalizar os valores de low e high
    while (true) {
        if (high < HALF) {
            // Caso 1: Intervalo está na metade inferior
            outputBits(0);
        } else if (low >= HALF) {
            // Caso 2: Intervalo está na metade superior
            outputBits(1);
            low -= HALF;
            high -= HALF;
        } else if (low >= FIRST_QTR && high < THIRD_QTR) {
            // Caso 3: Intervalo está no meio
            oppositeBits++;
            low -= FIRST_QTR;
            high -= FIRST_QTR;
        } else {
            break;
        }
        low <<= 1;
        high = (high << 1) | 1;
    }
}

void ArithmeticEncoder::outputBits(int bit) {
    // Escrever o bit atual e seus opostos pendentes
    writeBit(bit);
    while (oppositeBits > 0) {
        writeBit(!bit);
        oppositeBits--;
    }
}

void ArithmeticEncoder::writeBit(int bit) {
    // Armazenar os bits no buffer
    buffer = (buffer << 1) | bit;
    bitsInBuffer++;

    if (bitsInBuffer == 8) {
        // Escrever o byte completo no fluxo de saída
        output.put(buffer);
        bitsInBuffer = 0;
        buffer = 0; // Limpar buffer após escrever
    }
}

void ArithmeticEncoder::finishEncoding() {
    // Finalizar a codificação e garantir que todos os bits sejam escritos
    oppositeBits++;
    if (low < FIRST_QTR) {
        outputBits(0);
    } else {
        outputBits(1);
    }

    // Escrever qualquer dado restante no buffer (caso não tenha sido completado)
    if (bitsInBuffer > 0) {
        buffer <<= (8 - bitsInBuffer);  // Preencher com zeros os bits restantes
        output.put(buffer);
    }
}
