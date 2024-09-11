#ifndef LZW_H
#define LZW_H

#pragma once

#include <bits/stdc++.h>

class LZW {
    private:
        bool restartDictOnOverflow = true;
        std::map<std::string, u_int8_t> dictionary;
        int dictionaryMaxBytesCapacity = 0x1000;
        std::string lastPrefix;

        /* Atualiza o dicionario e prefixo para a proxima string do dicionario, de forma segura em relação a memoria */
        bool handleNewSymbol(u_int8_t symbol);
        
        bool restartDictionary();
        bool initializeDictionary();
    
    public:
        LZW();
        ~LZW();

        bool compress(std::string path);
        bool decompress(std::string path);

        bool loadDictionary(std::string path);
        bool saveDictionary(std::string path);

        bool setDictionaryMaxBytesCapacity(int capacity);
        int getDictionaryMaxBytesCapacity();

};

#endif