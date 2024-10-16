#ifndef LZW2_H
#define LZW2_H

#pragma once

#include <map>
#include <vector>
#include <cstdint>
#include <string>
#include <fstream>
#include <iostream>
#include "LZW2Writer.h"
#include "LZW2Reader.h"

class LZW2 {
public:
    LZW2();
    ~LZW2();

    // Setters
    void set_restart_map_on_overflow(bool b);
    void set_max_sequences(long int max_sequences);
    void set_benchmarking(bool is_benchmarking);
    void load_model(std::string& input_model_name);
    void save_model(std::string& output_model_name);

    void compress(std::string& input, std::string& output);
    void decompress(std::string& input, std::string& output);

private:
    std::map<std::vector<uint8_t>, lzw_code_t> compression_map;
    std::map<lzw_code_t, std::vector<uint8_t>> decompression_map;
    std::ofstream benchmark_data_file;

    unsigned long long max_sequences = LZW_CODE_T_MAX; // A princípio pode ser no máximo o que lzw_code_t puder comportar

    bool restart_map_on_overflow = true;
    bool is_using_model = false;
    bool is_benchmarking = false;

    void initialize_maps();
    long get_max_code_value();

};

#endif