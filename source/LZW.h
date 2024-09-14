#ifndef LZW_H
#define LZW_H

#pragma once

#include <bits/stdc++.h>

typedef uint16_t lzw_code_t;

class LZW {
    private:
        bool restartDictOnOverflow = true;
        std::unordered_map<std::string, lzw_code_t> compressionDict;
        std::unordered_map<lzw_code_t, std::string> decompressionDict;
        int dictionaryMaxBytesCapacity = 0x1000;
        std::string lastFoundSequence;
        lzw_code_t nextCode, previousCode, currentCode;

        std::ifstream input;
	    std::ofstream target;

        /* Atualiza o dicionario e prefixo para a proxima string do dicionario, de forma segura em relação a memoria */
        bool encodeSymbol(uint8_t symbol);
        bool decodeSymbol(lzw_code_t symbol);
        
        bool restartDictionary();
        bool initializeDictionaries();
    
    public:
        LZW();
        ~LZW();

        bool compress(std::string originalPath, std::string targetPath);
        bool decompress(std::string originalPath, std::string targetPath);

        bool loadDictionary(std::string originalPath);
        bool saveDictionary(std::string originalPath);

        bool setDictionaryMaxBytesCapacity(int capacity);
        int getDictionaryMaxBytesCapacity();

};

#endif