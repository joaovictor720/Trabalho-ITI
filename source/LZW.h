#ifndef LZW_H
#define LZW_H

#pragma once

#include <bits/stdc++.h>

typedef uint16_t lzw_code_t;

class LZW {
    private:
        std::unordered_map<std::string, lzw_code_t> compressionMap;
        std::unordered_map<lzw_code_t, std::string> decompressionMap;
        std::queue<std::pair<int, int>> benchmarkBuffer;

        std::ifstream input;
	    std::ofstream target;
        std::ofstream overallMeanBenchmarkFile;
        std::ofstream lastBytesMeanBenchmarkFile;
        std::string modelPath;
        
        lzw_code_t nextCompCode;
        lzw_code_t nextDecCode;
        bool restartDictOnOverflow = true;
        long int maxMapCapacity = pow(2, sizeof(lzw_code_t)*8)-1;
        int benchmarkBufferMaxSize = 1;
        bool benchmarking = false;
        long int currentBytesWritten = 0;
        long int encodedBytes = 0;

        bool usingTrainedModel = false;
        bool onTraining = false;

        bool encode(uint8_t symbol, std::string& currentString);
        bool decode(lzw_code_t symbol, std::string& previousString);
        
        bool handleBenchmarkData(long int bytesWritten, long int encodedBytes);
        bool updateCompMap(std::string newString);
        bool updateDecMap();
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