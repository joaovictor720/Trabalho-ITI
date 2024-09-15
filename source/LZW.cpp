#include "LZW.h"

LZW::LZW(bool benchmarking, int capacity, int benchmarkBufferSize, bool restartDict) : benchmarking(benchmarking), maxMapCapacity(capacity), benchmarkBufferMaxSize(benchmarkBufferSize), restartDictOnOverflow(restartDict)
{
	if (benchmarking)
		this->overallMeanBenchmarkFile = std::ofstream("overall_mean_benchmark.txt");
		this->lastBytesMeanBenchmarkFile = std::ofstream("last_bytes_mean_benchmark.txt");
		if (!this->overallMeanBenchmarkFile.is_open() || !this->lastBytesMeanBenchmarkFile.is_open()) {
			throw std::runtime_error("Could not open benchmark report files");
		}
}

LZW::~LZW() {}

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

bool LZW::handleBenchmarkData(long int bytesWritten, long int encodedBytes) {
	if (this->benchmarking) {
		// Salvando o comprimento médio atual
		this->overallMeanBenchmarkFile << std::fixed << std::setprecision(3) << ((double) bytesWritten * 8) / (double) encodedBytes << std::endl;

		// Colocando o novo par de dados no buffer do benchmark
		this->benchmarkBuffer.push(std::make_pair(bytesWritten, encodedBytes));

		// Se já tiver dados suficientes para o grupo de m bytes codificados
		if (benchmarkBuffer.size() == benchmarkBufferMaxSize) {
			std::queue<std::pair<int, int>> copy = benchmarkBuffer;
			double totalBitsWritten = 0, totalOriginalBytesEncoded = 0;

			// Somando os valores de bits escritos e bytes originais codificados
			while (!copy.empty()) {
				std::pair<int, int> pair = copy.front();
				totalBitsWritten += pair.first;
				totalOriginalBytesEncoded += pair.second;
				copy.pop();
			}

			// Salvando o comprimento médio associado aos últimos bytes originais codificados
			this->lastBytesMeanBenchmarkFile << std::fixed << std::setprecision(3) << double(totalBitsWritten) / double(totalOriginalBytesEncoded) << std::endl;
			benchmarkBuffer.pop();
		}
		return true;
	}
	return false;
}

bool LZW::encode(uint8_t symbol, std::string& currentSequence) {
	std::string newSequence = currentSequence + (char) symbol;
	this->encodedBytes++;
	if (compressionMap.find(newSequence) != compressionMap.end()) {
		currentSequence = newSequence;
	} else {
		target.write(reinterpret_cast<const char*>(&compressionMap[currentSequence]), sizeof(lzw_code_t));
		currentBytesWritten += sizeof(lzw_code_t);

		// Atualizando o dicionário (caso seja possível)
		if (this->compressionMap.size() <= this->maxMapCapacity) {
			this->compressionMap[newSequence] = this->nextCompCode++; // Adicionando string com o último símbolo lido
		} else {
			// Reiniciando o dicionário caso essa seja a estratégia escolhida para overflow
			if (this->restartDictOnOverflow) {
				this->initializeMaps();
			}
		}
		currentSequence = symbol; // Reiniciando string atual
	}
	this->handleBenchmarkData(currentBytesWritten, encodedBytes);
	return true;
}

bool LZW::compress(std::string inputPath, std::string targetPath) {
	this->input = std::ifstream(inputPath);
	this->target = std::ofstream(targetPath, std::ios::binary);
	this->initializeMaps();

    std::string currentSequence;
    while (!input.eof()) {
		uint8_t byte = input.get();
        this->encode(byte, currentSequence);
    }

	std::cout << "Max cap: " << this->maxMapCapacity << std::endl;

    if (!currentSequence.empty()) {
		target.write(reinterpret_cast<const char*>(&compressionMap[currentSequence]), sizeof(lzw_code_t));
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
	} else {
		// Caso especial
		currentString = previousString + previousString[0];
	}

	target << currentString;

	if (this->decompressionMap.size() <= this->maxMapCapacity) {
		decompressionMap[this->nextDecCode++] = previousString + currentString[0];
	} else {
		if (restartDictOnOverflow) {
			this->initializeMaps();
		}
	}
	previousString = currentString;

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