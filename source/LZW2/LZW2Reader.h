#ifndef LZW2READER_H
#define LZW2READER_H

#pragma once

#include <fstream>
#include <string>

class LZW2Reader
{
public:
    LZW2Reader(std::string& input);
    ~LZW2Reader();

    lzw_code_t read();
    bool eof(); // Sinalizar aqui quando terminar de ler o arquivo
    void close();

private:
    std::ifstream input;
    int code_width = 8; // Começa com 8 para comportar todos os valores de um byte

    long get_max_code_value();
};

#endif