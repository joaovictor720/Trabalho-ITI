#ifndef LZW2WRITER_H
#define LZW2WRITER_H

#pragma once

#include <fstream>
#include <string>
#include "LZWType.h"

class LZW2Writer
{
public:
    LZW2Writer(std::string& output);
    ~LZW2Writer();

    void write(lzw_code_t code);
    long get_max_code_value();
    void increment_width();
    void flush();
    void close();

private:
    std::ofstream output;
    int code_width = 8; // Começa com 8 para comportar todos os valores de um byte
    int buf_size = 0;
    u_int64_t buf = 0;
};

#endif