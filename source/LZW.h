#ifndef LZW_H
#define LZW_H

#pragma once

#include <bits/stdc++.h>

typedef uint16_t lzw_code_t;

class LZW {
    private:
        std::unordered_map<std::string, lzw_code_t> compressionMap;
        std::unordered_map<lzw_code_t, std::string> decompressionMap;
        lzw_code_t nextCompCode, nextDecCode;
        
        bool restartDictOnOverflow = true;
        // int maxMapCapacity = pow(2, sizeof(lzw_code_t)*8);
        int maxMapCapacity = 0x1000;

        long int currentBytesWritten = 0, encodedBytes = 0;
        std::queue<double> bytesWritten;

        std::ifstream input;
	    std::ofstream target;

        /* Atualiza o dicionario e prefixo para a proxima string do dicionario, de forma segura em relação a memoria */
        bool encode(uint8_t symbol, std::string& currentString);
        bool decode(lzw_code_t symbol, std::string& previousString);
        
        bool restartDictionary();
        bool initializeMaps();
    
    public:
        LZW();
        ~LZW();

        bool compress(std::string inputPath, std::string targetPath);
        bool decompress(std::string inputPath, std::string targetPath);

        bool loadDictionary(std::string inputPath);
        bool saveDictionary(std::string inputPath);
        bool dumpBytesWritten();

        bool setDictionaryMaxBytesCapacity(int capacity);
        int getDictionaryMaxBytesCapacity();

};

#endif