#ifndef PPMC_H
#define PPMC_H

#pragma once

#include "TrieNode.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include "ArithmeticEncoder.h"
#include "ArithmeticDecoder.h"

class PPMc
{
public:
    PPMc(int maxContext);
    ~PPMc();

    void compressFile(const std::string& inputFile, const std::string& outputFile);
    void encodeAndUpdateContexts(uint16_t symbol, ArithmeticEncoder& encoder, const std::vector<uint16_t>& context);

    void decompressFile(const std::string& inputFile, const std::string& outputFile);

private:
    TrieNode* root;
    int maxContext;
    std::vector<std::pair<uint16_t, int>> equiprobableSymbols;

    int findMaxExistentContextAndUpdateInexistents(uint16_t symbol, const std::vector<uint16_t>& context);
    void initializeEquiprobableSymbols();

    void subtractFrequenciesVector(std::vector<std::pair<uint16_t, int>>& currentFrequencies, const std::vector<std::pair<uint16_t, int>>& rejectedFrequencies);
    void addFrequenciesVector(std::vector<std::pair<uint16_t, int>>& globalRejectedFrequencies, const std::vector<std::pair<uint16_t, int>>& rejectedFrequencies);

    void insertSymbol(const std::vector<uint16_t>& context, uint16_t symbol);
    void updateContextAndSymbol(const std::vector<uint16_t>& context, uint16_t symbol);
    std::vector<std::pair<uint16_t, int>> getSortedFrequencies(const std::vector<uint16_t>& context);


    bool decodeAndUpdateContexts(ArithmeticDecoder& decoder, std::vector<uint16_t>& context, std::ofstream& output);


    ////////////////////////////

    int findMaxExistentContext(const std::vector<uint16_t>& context);
    void updateAllTables(uint16_t symbol, int levelKOfCodifiedSymbol, const std::vector<uint16_t>& context);
};

#endif