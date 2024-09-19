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
    PPMc(int maxContext, uint64_t maxBytes);
    ~PPMc();

    void compressFile(const std::string& inputFile, const std::string& outputFile);
    void encodeAndUpdateContexts(uint16_t symbol, ArithmeticEncoder& encoder, const std::vector<uint16_t>& context);

    void decompressFile(const std::string& inputFile, const std::string& outputFile);
    uint64_t bytesAddedToModel;   // Contador de bytes adicionados até o momento

    void saveModel(const std::string& modelFile);////////////////////////////
    void loadModel(const std::string& modelFile);////////////////////
private:
    TrieNode* root;
    int maxContext;
    uint64_t maxBytes;            // Limite máximo de bytes a serem adicionados ao modelo
    std::vector<std::pair<uint16_t, int>> equiprobableSymbols;
    uint64_t pastBitWrited;

    int findMaxExistentContextAndUpdateInexistents(uint16_t symbol, const std::vector<uint16_t>& context);
    void initializeEquiprobableSymbols();

    void subtractFrequenciesVector(std::vector<std::pair<uint16_t, int>>& currentFrequencies, const std::vector<std::pair<uint16_t, int>>& rejectedFrequencies);
    void addFrequenciesVector(std::vector<std::pair<uint16_t, int>>& globalRejectedFrequencies, const std::vector<std::pair<uint16_t, int>>& rejectedFrequencies);
    std::vector<std::pair<uint16_t, int>> getSortedFrequencies(const std::vector<uint16_t>& context);


    bool decodeAndUpdateContexts(ArithmeticDecoder& decoder, std::vector<uint16_t>& context, std::ofstream& output);


    ////////////////////////////

    int findMaxExistentContext(const std::vector<uint16_t>& context);
    void updateAllTables(uint16_t symbol, int levelKOfCodifiedSymbol, const std::vector<uint16_t>& context);
    void serializeTrie(TrieNode* root, std::ofstream& outFile);
    TrieNode* deserializeTrie(std::ifstream& inFile);
    void serializeModel(TrieNode* root, const std::vector<std::pair<uint16_t, int>>& equiprobableSymbols, std::ofstream& outFile);
    void deserializeModel(std::ifstream& inFile);


    ///////////////////

    void saveCompressionData(uint64_t n, double l_n, const std::string& filename);
};

#endif