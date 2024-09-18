#ifndef LZW_H
#define LZW_H

#pragma once

#include <bits/stdc++.h>
#include "BitWriter.h"
#include "BitReader.h"

class LZW {
    private:
        std::map<std::vector<uint8_t>, lzw_code_t> compressionMap;
        std::map<lzw_code_t, std::vector<uint8_t>> decompressionMap;
        std::queue<std::pair<int, int>> benchmarkBuffer;

        std::ifstream input;
	    std::ofstream target;
        std::ofstream overallMeanBenchmarkFile;
        std::ofstream lastBytesMeanBenchmarkFile;
        std::string modelPath;
        
        bool restartMapOnOverflow = true;
        long int maxMapCapacity = pow(2, sizeof(lzw_code_t)*8)-1;
        int benchmarkBufferMaxSize = 1;
        bool benchmarking = false;
        long int currentBytesWritten = 0;
        long int encodedBytes = 0;
        int bitWidth = 8; // Start with 8 bits for 256 codes
        int maxCodeValue = (1 << bitWidth) - 1;

        bool usingTrainedModel = false;
        bool onTraining = false;

        bool encode(uint8_t symbol, std::vector<uint8_t>& currentString, BitWriter& writer);
        bool decode(lzw_code_t symbol, std::vector<uint8_t>& previousString, BitReader& reader);
        
        bool handleBenchmarkData(long int bytesWritten, long int encodedBytes);
        bool initializeMaps();
    
    public:
        LZW(bool benchmarking, int capacity, int benchmarkBufferSize, bool restartDict, bool trainingMode);
        ~LZW();

        bool compress(std::string inputPath, std::string targetPath);
        bool decompress(std::string inputPath, std::string targetPath);

        bool saveCompressionMap(std::string modelFilename);
        bool usingModel(std::string modelPath);
};

#endif