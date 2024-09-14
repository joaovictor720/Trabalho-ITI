#include "LZW.h"

std::string replaceExtension(const std::string& filePath, std::string newExtension) {
    // Find the last dot in the file originalPath
    size_t dotPosition = filePath.find_last_of('.');

    // If no dot is found or it's part of the directory, return the original string
    if (dotPosition == std::string::npos || filePath.find_last_of("/\\") > dotPosition) {
        return filePath + newExtension; // Add .jvav extension if no dot or extension found
    }

    // Replace the extension with .jvav
    return filePath.substr(0, dotPosition) + newExtension;
}

LZW::LZW()
{

}

LZW::~LZW()
{

}

bool LZW::initializeDictionaries() {
	// std::cout << "INITIALIZING DICTIONARY\n";
	compressionDict.clear();
	for (nextCode = 0; nextCode < 0x100; nextCode++) {
		compressionDict[std::string(1, (char) nextCode)] = nextCode;
		// std::cout << "key" << std::string(1, nextCode) << std::endl;
		// std::cout << "value" << nextCode << std::endl;
	}
	decompressionDict.clear();
	for (nextCode = 0; nextCode < 0x100; nextCode++) {
		decompressionDict[nextCode] = std::string(1, (char) nextCode);
		// std::cout << "key" << std::string(1, nextCode) << std::endl;
		// std::cout << "value" << nextCode << std::endl;
	}
	// for (auto& pair : decompressionDict) {
	// 	std::cout << pair.first << "<>" << pair.second << std::endl;
	// }
	return true;
}

bool LZW::encodeSymbol(uint8_t symbol) {
	std::string currentSequence = this->lastFoundSequence + std::string(1, symbol);

	// std::cout << "\nCurrent symb ";
	// std::cout << std::hex << (int) symbol << std::endl;
	// std::cout << "Current seq " << currentSequence << std::endl;

	if (this->compressionDict.count(currentSequence) > 0) {
		this->lastFoundSequence = currentSequence;
	} else {
		// std::cout << "Sending " << compressionDict[lastFoundSequence] << std::endl;
		target.write(reinterpret_cast<const char*>(&compressionDict[lastFoundSequence]), sizeof(lzw_code_t));
		compressionDict[currentSequence] = currentCode++;
		this->lastFoundSequence = std::string(1, symbol);
	}
	return true;
}

bool LZW::decodeSymbol(lzw_code_t encodedSymbol) {
	std::cout << "\nRead " << std::hex << encodedSymbol << std::endl;
	std::cout << "Associated string " << decompressionDict[encodedSymbol] << std::endl;
	
	// if (this->decompressionDict.count(encodedSymbol) > 0) {
	// 	this->lastFoundSequence = decompressionDict[encodedSymbol];
	// } else {
	// 	this->lastFoundSequence = decompressionDict[encodedSymbol] + decompressionDict[previousCode].at(0);
	// }

	// target << this->lastFoundSequence;
	// std::cout << "Wrote " << this->lastFoundSequence << std::endl;
	// this->decompressionDict[this->nextCode++] = this->decompressionDict[previousCode] + this->lastFoundSequence[0];
	// std::cout << "New string " << this->decompressionDict[nextCode-1] << std::endl;
	// this->previousCode = this->nextCode;
	// std::cout << "Next code " << this->nextCode << std::endl;

	if (this->decompressionDict.count(encodedSymbol) > 0) {
		std::string temp = decompressionDict[encodedSymbol];
		target << temp;
		decompressionDict[nextCode] = decompressionDict[previousCode] + temp[0];
		nextCode++;
		previousCode = encodedSymbol;
	} else {
		std::string temp = decompressionDict[previousCode];
		decompressionDict[nextCode] = temp + temp.at(0);
		nextCode++;
		target << temp + temp.at(0);
	}

	return true;
}

bool LZW::compress(std::string originalPath, std::string targetPath) {
	std::cout << "Comprimindo: " << originalPath << "..." << std::endl;
	this->input = std::ifstream(originalPath);
	this->target = std::ofstream(targetPath, std::ios::binary);
	// this->initializeDictionaries();

	// uint8_t byte;
	// this->lastFoundSequence = "";
	// while (!input.eof()) {
	// 	byte = this->input.get();
	// 	this->encodeSymbol(byte);
	// }

	// if (compressionDict[lastFoundSequence]) {
	// 	lzw_code_t code = compressionDict[lastFoundSequence];
	// 	target.write(reinterpret_cast<const char*>(&code), sizeof(lzw_code_t));
	// }

    // std::unordered_map<std::string, lzw_code_t> compressionDict;
    int dictSize = 256;
    
    // for (int i = 0; i < 256; i++) {
    //     dictionary[std::string(1, char(i))] = i;
    // }
	this->initializeDictionaries();

    std::string currentString;

    while (!input.eof()) {
		char character = input.get();
        std::string newString = currentString + character;
        if (compressionDict.find(newString) != compressionDict.end()) {
            currentString = newString;
        } else {
			target.write(reinterpret_cast<const char*>(&compressionDict[currentString]), sizeof(lzw_code_t));
            compressionDict[newString] = dictSize++;
            currentString = character;
        }
    }

    if (!currentString.empty()) {
		target.write(reinterpret_cast<const char*>(&compressionDict[currentString]), sizeof(lzw_code_t));
    }

	this->input.close();
	this->target.flush();
	this->target.close();
	return true;
}

bool LZW::decompress(std::string compressedPath, std::string originalPath) {
	std::cout << "Descomprimindo: " << compressedPath << "..." << std::endl;
	// this->input = std::ifstream(compressedPath, std::ios::binary);
	// this->target = std::ofstream(originalPath);
	// this->initializeDictionaries();

	// input.read(reinterpret_cast<char*>(&this->currentCode), sizeof(lzw_code_t));
	// if (decompressionDict.count(currentCode) > 0) {
	// 	std::cout << "Writing " << decompressionDict[currentCode] << " from " << std::hex << currentCode << std::dec << std::endl;
	// 	target << decompressionDict[this->currentCode];
	// } else {
	// 	std::cout << "String from " << std::hex << currentCode << std::dec << " not present" << std::endl;
	// }
	// this->previousCode = this->currentCode;

	// while (!input.eof()) {
	// 	input.read(reinterpret_cast<char*>(&this->currentCode), sizeof(lzw_code_t));
	// 	this->decodeSymbol(this->currentCode);
	// }
	// for (auto& pair : decompressionDict) {
	// 	std::cout << std::dec << pair.first << "<>" << pair.second << std::endl;
	// }

	// Initialize the dictionary with single-character entries
    std::unordered_map<lzw_code_t, std::string> dictionary;
    int dictSize = 256;
    
    for (int i = 0; i < 256; i++) {
        dictionary[i] = std::string(1, char(i));
    }

    // Get the first code and initialize previousString
	input = std::ifstream(compressedPath, std::ios::binary);
	target = std::ofstream(originalPath);

	lzw_code_t code;
	input.read(reinterpret_cast<char*>(&code), sizeof(lzw_code_t));
    std::string previousString = dictionary[code];
	target << previousString;

    std::string currentString;
	while (!input.eof()) {
		input.read(reinterpret_cast<char*>(&code), sizeof(lzw_code_t));

        // If the current code is in the dictionary
        if (dictionary.find(code) != dictionary.end()) {
            currentString = dictionary[code];
        } else if (code == dictSize) {
            // Special case where currentString is the same as previousString + first character of previousString
            currentString = previousString + previousString[0];
        } else {
            throw std::runtime_error("Decompression error: Invalid code encountered.");
        }

        // Output currentString
		target << currentString;

        // Add new sequence to the dictionary (previousString + first character of currentString)
        dictionary[dictSize++] = previousString + currentString[0];

        // Update previousString
        previousString = currentString;
    }

	this->input.close();
	this->target.flush();
	this->target.close();

	return true;
}