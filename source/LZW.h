#ifndef LZW_H
#define LZW_H

#pragma once

#include <bits/stdc++.h>

class LZW {
    private:
        bool restartDictOnOverflow = true;
        std::map<std::string, int> dict;
        int dictMaxBytesCapacity = 0x1000;
        std::string lastPrefix;

        /* Atualiza o dicionario e prefixo para a proxima string do dicionario, de forma segura em relação a memoria */
        bool handleNewSymbol(unsigned char symbol); 
        bool restartDictionary();
    
    public:
        LZW();
        ~LZW();

};

#endif