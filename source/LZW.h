#ifndef LZW_H
#define LZW_H

#pragma once

#include <bits/stdc++.h>

typedef u_int16_t lzw_code_t;

class LZW {
    private:
        bool restartDictOnOverflow = true;
        std::map<std::string, lzw_code_t> dictionary;
        int dictionaryMaxBytesCapacity = 0x1000;
        std::string lastFoundSequence;
        lzw_code_t lastDictValue = 0;

        std::ifstream input;
	    std::ofstream target;

        /* Atualiza o dicionario e prefixo para a proxima string do dicionario, de forma segura em relação a memoria */
        bool handleSymbol(u_int8_t symbol);
        
        bool restartDictionary();
        bool initializeDictionary();
    
    public:
        LZW();
        ~LZW();

        bool compress(std::string originalPath, std::string targetPath);
        bool decompress(std::string originalPath);

        bool loadDictionary(std::string originalPath);
        bool saveDictionary(std::string originalPath);

        bool setDictionaryMaxBytesCapacity(int capacity);
        int getDictionaryMaxBytesCapacity();

};

#endif