#include "LZW.h"

LZW::LZW()
{

}

LZW::~LZW()
{

}

bool LZW::dumpBytesWritten() {
	std::ofstream benchmarkFile("benchmark.txt");
	int subSampleRate = 10, i = 0;
	while (!bytesWritten.empty()) {
		double sample = bytesWritten.front();
		bytesWritten.pop();
		if (i++ % subSampleRate == 0)
			benchmarkFile << std::fixed << std::setprecision(3) << sample << std::endl;
	}
	benchmarkFile.close();
	return true;
}

bool LZW::initializeMaps() {
	compressionMap.clear();
	for (nextCompCode = 0; nextCompCode < 0x100; nextCompCode++) {
		compressionMap[std::string(1, (char) nextCompCode)] = nextCompCode;
	}
	decompressionMap.clear();
	for (nextDecCode = 0; nextDecCode < 0x100; nextDecCode++) {
		decompressionMap[nextDecCode] = std::string(1, (char) nextDecCode);
	}
	return true;
}

bool LZW::encode(uint8_t symbol, std::string& currentString) {
	std::string newString = currentString + (char) symbol;
	this->encodedBytes++;
	if (compressionMap.find(newString) != compressionMap.end()) {
		currentString = newString;
	} else {
		target.write(reinterpret_cast<const char*>(&compressionMap[currentString]), sizeof(lzw_code_t));
		currentBytesWritten += sizeof(lzw_code_t);
		// if (this->nextCompCode > this->maxMapCapacity) {
		// 	// Reiniciando dicionários
		// 	this->initializeMaps();
		// 	newString = (char) symbol;
		// }
		compressionMap[newString] = this->nextCompCode++; // Adicionando string com o último símbolo lido
		currentString = symbol; // Reiniciando string atual
	}
	// std::cout << currentBytesWritten*8 << " / " << encodedBytes << " = " << currentBytesWritten*8 / encodedBytes << std::endl;
	bytesWritten.push(((double) currentBytesWritten * 8) / (double) encodedBytes);
	return true;
}

bool LZW::compress(std::string inputPath, std::string targetPath) {
	std::cout << "Compressing: " << inputPath << "..." << std::endl;
	std::cout << "-> Encoded symbol size: " << sizeof(lzw_code_t)*8 << " bits" << std::endl;
	this->input = std::ifstream(inputPath);
	this->target = std::ofstream(targetPath, std::ios::binary);
	this->initializeMaps();

    std::string currentString;
    while (!input.eof()) {
		uint8_t byte = input.get();
        this->encode(byte, currentString);
    }

    if (!currentString.empty()) {
		target.write(reinterpret_cast<const char*>(&compressionMap[currentString]), sizeof(lzw_code_t));
    }

	this->input.close();
	this->target.flush();
	this->target.close();
	return true;
}

bool LZW::decode(lzw_code_t encodedSymbol, std::string& previousString) {
	std::string currentString;
	if (encodedSymbol == 0xff) {
		// Não decodificar o EOF
		return true;
	}

	if (decompressionMap.find(encodedSymbol) != decompressionMap.end()) {
		currentString = decompressionMap[encodedSymbol];
	} else if (encodedSymbol == this->nextDecCode) {
		currentString = previousString + previousString[0];
	} else {
		throw std::runtime_error("Decompression error: Invalid code encountered.");
	}

	target << currentString;

	// if (this->nextDecCode > this->maxMapCapacity) {
	// 	this->initializeMaps();
	// } else {
		decompressionMap[this->nextDecCode++] = previousString + currentString[0];
		previousString = currentString;
	// }

	return true;
}

bool LZW::decompress(std::string inputPath, std::string targetPath) {
	std::cout << "Decompressing: " << inputPath << "..." << std::endl;

	input = std::ifstream(inputPath, std::ios::binary);
	target = std::ofstream(targetPath);
	this->initializeMaps();
    this->nextDecCode = decompressionMap.size();

	lzw_code_t code;
	input.read(reinterpret_cast<char*>(&code), sizeof(lzw_code_t));
    std::string previousString = decompressionMap[code];
	target << previousString;

	while (!input.eof()) {
		input.read(reinterpret_cast<char*>(&code), sizeof(lzw_code_t));
		this->decode(code, previousString);
    }

	this->input.close();
	this->target.flush();
	this->target.close();

	return true;
}