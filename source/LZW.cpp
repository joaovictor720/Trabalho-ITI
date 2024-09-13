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

bool LZW::initializeDictionary() {
	// std::cout << "INITIALIZING DICTIONARY\n";
	for (lastDictValue = 0; lastDictValue < 0x100; lastDictValue++) {
		dictionary[std::string(1, lastDictValue)] = lastDictValue;
		// std::cout << "key" << std::string(1, lastDictValue) << std::endl;
		// std::cout << "value" << lastDictValue << std::endl;
	}
	return true;
}

bool LZW::handleSymbol(u_int8_t symbol) {
	std::string currentSequence = this->lastFoundSequence + std::string(1, symbol);

	// std::cout << "\nCurrent symb ";
	// std::cout << std::hex << (int) symbol << std::endl;
	// std::cout << "Current seq " << currentSequence << std::endl;

	if (this->dictionary[currentSequence]) {
		this->lastFoundSequence = currentSequence;
	} else {
		// std::cout << "Sending " << dictionary[lastFoundSequence] << std::endl;
		target.write(reinterpret_cast<const char*>(&dictionary[lastFoundSequence]), sizeof(lzw_code_t));
		dictionary[currentSequence] = lastDictValue++;
		this->lastFoundSequence = std::string(1, symbol);
	}
	return true;
}

bool LZW::compress(std::string originalPath, std::string targetPath) {
	this->input = std::ifstream(originalPath);
	this->target = std::ofstream(targetPath, std::ios::binary);
	this->initializeDictionary();

	u_int8_t byte;
	this->lastFoundSequence = "";
	while (!input.eof()) {
		byte = this->input.get();
		// printf("%x\n", byte);
		this->handleSymbol(byte);
	}

	if (dictionary[lastFoundSequence]) {
		lzw_code_t code = dictionary[lastFoundSequence];
		target.write(reinterpret_cast<const char*>(&code), sizeof(lzw_code_t));
	}

	// for (auto& pair : this->dictionary) {
	// 	std::cout << pair.first << "<->" << pair.second << std::endl;
	// }

	this->input.close();
	this->target.flush();
	this->target.close();
	return true;
}