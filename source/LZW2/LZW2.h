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

typedef uint32_t lzw_code_t;

class LZW2 {
public:
    LZW2();
    ~LZW2();

    // Setters
    void set_restart_map_on_overflow(bool b);
    void set_max_phrases(long int max_phrases);

    void compress(std::string& input, std::string& output);
    void decompress(std::string& input, std::string& output);

private:
    std::map<std::vector<uint8_t>, lzw_code_t> compression_map;
    std::map<lzw_code_t, std::vector<uint8_t>> decompression_map;

    long int max_phrases = (1 << sizeof(lzw_code_t)*8) - 1; // A princípio pode ser no máximo o que lzw_code_t puder comportar

    bool restart_map_on_overflow = true;

    void initialize_maps();
    long get_max_code_value();

};

#endif