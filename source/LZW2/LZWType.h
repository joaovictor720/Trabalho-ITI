#ifndef LZWTYPE_H
#define LZWTYPE_H

#pragma once

#include <iostream>
#include <cmath>
#include <fstream>
#include <string>

#define BYTE_SIZE 8

typedef uint64_t lzw_code_t;
#define LZW_CODE_T_MAX (1 << sizeof(lzw_code_t)*8) - 1

#endif