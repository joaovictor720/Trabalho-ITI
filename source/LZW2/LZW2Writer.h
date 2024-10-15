#ifndef LZW2WRITER_H
#define LZW2WRITER_H

#pragma once

#include "LZWType.h"

class LZW2Writer
{
public:
    LZW2Writer(std::string& output);
    ~LZW2Writer();

    void write(lzw_code_t code);
    long get_max_code_value();
    void increment_width();
    void set_code_width_from_map_size(int size);
    void set_benchmarking(bool is_benchmarking);
    unsigned long long get_bits_written();
    void flush();
    void close();

private:
    std::ofstream output;
    std::ofstream benchmark_data_file;
    int code_width = 8; // Começa com 8 para comportar todos os valores de um byte
    int buf_size = 0;
    u_int64_t buf = 0;
    unsigned long long bits_written = 0;
    bool is_benchmarking = false;
};

#endif