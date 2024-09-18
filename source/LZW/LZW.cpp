#include "LZW.h"

LZW::LZW(bool benchmarking, int capacity, int benchmarkBufferSize, bool restartDict, bool trainingMode)
{
	this->benchmarking = benchmarking;
	this->maxMapCapacity = capacity;
	this->benchmarkBufferMaxSize = benchmarkBufferSize;
	this->restartMapOnOverflow = restartDict;
	this->onTraining = trainingMode;
	if (benchmarking) {
		this->overallMeanBenchmarkFile = std::ofstream("overall_mean_benchmark.txt");
		this->lastBytesMeanBenchmarkFile = std::ofstream("last_bytes_mean_benchmark.txt");
		if (!this->overallMeanBenchmarkFile.is_open() || !this->lastBytesMeanBenchmarkFile.is_open()) {
			std::cerr << "Could not open benchmark report files" << std::endl;
			return;
		}
	}
}

LZW::~LZW() {}

bool LZW::saveCompressionMap(std::string modelFilename) {
	std::ofstream outFile(modelFilename, std::ios::binary);

    if (!outFile.is_open()) {
        std::cerr << "Failed to open file for writing." << std::endl;
        return false;
    }

    // Write the size of the map
    size_t mapSize = this->compressionMap.size();
    outFile.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize));

    // Write each key-value pair to the file
    for (const auto& pair : this->compressionMap) {
        // Write the size of the vector (key)
        size_t keySize = pair.first.size();
        outFile.write(reinterpret_cast<const char*>(&keySize), sizeof(keySize));

        // Write the vector (key) bytes
        outFile.write(reinterpret_cast<const char*>(pair.first.data()), keySize);

        // Write the value
        outFile.write(reinterpret_cast<const char*>(&pair.second), sizeof(pair.second));
    }

    outFile.close();
	std::cout << "Map written to " << modelFilename << " successfully." << std::endl;
	return true;
}

bool LZW::usingModel(std::string modelPath) {
	std::cout << "Loading model from " << modelPath << std::endl;
	std::ifstream modelFile(modelPath, std::ios::binary);

	if (compressionMap.size() != 0 && decompressionMap.size() != 0) {
		return true;
	}

    if (!modelFile.is_open()) {
        std::cerr << "Failed to open file for reading." << std::endl;
        return false;
    }

    // Read the size of the map
    size_t mapSize;
    modelFile.read(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));

    // Read each key-value pair from the file
    for (size_t i = 0; i < mapSize; ++i) {
        // Read the size of the vector (key)
        size_t keySize;
        modelFile.read(reinterpret_cast<char*>(&keySize), sizeof(keySize));

        // Read the vector (key) bytes
        std::vector<uint8_t> key(keySize);
        modelFile.read(reinterpret_cast<char*>(key.data()), keySize);

        // Read the value
        lzw_code_t value;
        modelFile.read(reinterpret_cast<char*>(&value), sizeof(value));

        // Insert the pair into the map
        this->compressionMap[key] = value;
        this->decompressionMap[value] = key;
    }

    modelFile.close();
    std::cout << "Map read from " << modelPath << " successfully." << std::endl;
	return true;
}

bool LZW::initializeMaps() {
	// Se não estiver usando um modelo já pronto, aí sim mexer nos dicionários
	if (!this->usingTrainedModel) {
		this->compressionMap.clear();
		for (lzw_code_t nextCompCode = 0; nextCompCode < 0x100; nextCompCode++) {
			this->compressionMap[{static_cast<uint8_t>(nextCompCode)}] = nextCompCode;
		}
		this->decompressionMap.clear();
		for (lzw_code_t nextDecCode = 0; nextDecCode < 0x100; nextDecCode++) {
			this->decompressionMap[nextDecCode] = {static_cast<uint8_t>(nextDecCode)};
		}
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

bool LZW::encode(uint8_t symbol, std::vector<uint8_t>& currentSequence, BitWriter& writer) {
	std::vector<uint8_t> newSequence = currentSequence;
	newSequence.push_back(symbol);
	this->encodedBytes++; // Conta total de bytes originais processados
	if (compressionMap.find(newSequence) != compressionMap.end()) {
		currentSequence = newSequence;
	} else {
		if (!this->onTraining) {
			// std::cout << "writing in ";
			// for (auto& c : currentSequence) {
			// 	// std::bitset<8> temp = c;
			// 	// std::cout << temp << " = " << temp.to_ulong() << std::endl;
			// 	std::cout << (int) c << "-";
			// }
			// std::cout << std::dec << " || " << (int) compressionMap[currentSequence];
			// std::cout << "\n";
			// writer.writeCode(compressionMap[currentSequence], this->bitWidth);
			// writer.addCode(compressionMap[currentSequence], this->bitWidth);
			target.write(reinterpret_cast<const char*>(&compressionMap[currentSequence]), sizeof(lzw_code_t));
		}
		// currentBytesWritten += sizeof(lzw_code_t);

		if (this->compressionMap.size() > this->maxCodeValue) {
			this->bitWidth++;
			this->maxCodeValue = (1 << bitWidth) - 1;
		}

		// Atualizando o dicionário (caso seja possível)
		if (this->compressionMap.size() <= this->maxMapCapacity) {
			// std::cout << "adding to map ";
			// for (auto& b : newSequence) {
			// 	std::cout << (int) b << "-";
			// }
			// std::cout << std::dec << " || " << (int) compressionMap.size() << std::endl;
			this->compressionMap[newSequence] = this->compressionMap.size(); // Adicionando string com o contador como codigo
		} else {
			if (this->onTraining) {
				this->saveCompressionMap("lzw_model.txt");
				return true;
			}
			// Reiniciando o dicionário caso essa seja a estratégia escolhida para overflow
			if (this->restartMapOnOverflow) {
				this->initializeMaps();
			}
		}
		currentSequence.clear();
		currentSequence.push_back(symbol); // Reiniciando a sequência atual
	}
	this->handleBenchmarkData(currentBytesWritten, encodedBytes);
	return true;
}

bool LZW::compress(std::string inputPath, std::string targetPath) {
	this->input = std::ifstream(inputPath, std::ios::binary);
	this->target = std::ofstream(targetPath, std::ios::binary);
	// BitWriter writer(targetPath);
	BitWriter writer;
	
	this->initializeMaps();

    std::vector<uint8_t> currentSequence;
	char byte;
    while (input.get(byte)) {
		
		uint8_t symbol = static_cast<uint8_t>(byte);
		// std::bitset<8> temp = symbol;
		// std::cout << "original byte " << temp << " = " << temp.to_ulong() << std::endl;
        this->encode(symbol, currentSequence, writer);
		// for (auto& pair : compressionMap) {
		// 	for (auto& b : pair.first) {
		// 		std::cout << std::hex << (int) b << "-";
		// 	}
		// 	std::cout << std::dec << " || " << (int) pair.second << std::endl;
		// }
    }

    if (!currentSequence.empty()) {
		// std::cout << "writing in ";
		// for (auto& c : currentSequence) {
		// 	// std::bitset<8> temp = c;
		// 	// std::cout << temp << " = " << temp.to_ulong() << std::endl;
		// 	std::cout << (int) c << "-";
		// }
		// std::cout << std::dec << " || " << (int) compressionMap[currentSequence];
		// std::cout << "\n";
		target.write(reinterpret_cast<const char*>(&compressionMap[currentSequence]), sizeof(lzw_code_t));
    }
	// writer.flush();
	// if (!currentSequence.empty())
	// 	writer.addCode(this->compressionMap[currentSequence], this->bitWidth);

	writer.writeToFile(targetPath);

	this->input.close();
	this->target.flush();
	this->target.close();
	return true;
}

bool LZW::decode(lzw_code_t encodedSymbol, std::vector<uint8_t>& previousSequence, BitReader& reader) {
	std::vector<uint8_t> currentSequence;

	if (decompressionMap.find(encodedSymbol) != decompressionMap.end()) {
		currentSequence = this->decompressionMap[encodedSymbol];
	} else {
		// Caso especial em que a sequência do símbolo ainda não está totalmente formada
		std::vector<uint8_t> temp = previousSequence;
		temp.push_back(previousSequence[0]);
		currentSequence = temp;
	}

	// Adjust bit width if the dictionary grows too large
	if (this->decompressionMap.size() > this->maxCodeValue) {
		this->bitWidth++;
		this->maxCodeValue = (1 << bitWidth) - 1;
	}

	// std::cout << "size " << decompressionMap.size() << ", bitWidth " << this->bitWidth;
	// std::cout << ", writing back ";
	// std::cout << std::dec << (int) encodedSymbol << " || ";
	// for (auto& c : currentSequence) {
	// 	// std::bitset<8> temp = c;
	// 	// std::cout << temp << " = " << temp.to_ulong() << std::endl;
	// 	std::cout << (char) c << "-";
	// }
	// std::cout << "\n";
	this->target.write(reinterpret_cast<const char*>(currentSequence.data()), currentSequence.size());

	// Atualizando o dicionário (se possível)
	if (this->decompressionMap.size() <= this->maxMapCapacity) {
		std::vector<uint8_t> temp = previousSequence;
		temp.push_back(currentSequence[0]);
		// std::cout << "adding to map ";
		// std::cout << std::dec << (int) decompressionMap.size() << " || ";
		// for (auto& b : temp) {
		// 	std::cout << (int) b << "-";
		// }
		// std::cout << std::endl;
		
		this->decompressionMap[this->decompressionMap.size()] = temp;
	} else {
		if (this->restartMapOnOverflow) {
			this->initializeMaps();
		}
	}
	previousSequence = currentSequence;

	return true;
}

bool LZW::decompress(std::string inputPath, std::string targetPath) {
	if (this->onTraining) {
		return false;
	}

	this->input = std::ifstream(inputPath, std::ios::binary);
	this->target = std::ofstream(targetPath, std::ios::binary);
    BitReader reader;
	std::cout << "Carregando o arquivo " << inputPath << " para descompressão" << std::endl;
	reader.readFromFile(inputPath);

    // Initialize dictionary and bit width
    this->bitWidth = 8;  // Start reading 8-bit codes
    this->maxCodeValue = (1 << bitWidth) - 1;
	if (!this->input.is_open() || !this->target.is_open()) {
		std::cerr << "Could not open files for decompression." << std::endl;
		return false;
	}

	this->initializeMaps();
    // this->nextDecCode = this->decompressionMap.size();

	// std::cout << "\nSending bitWidth " << this->bitWidth << std::endl;
	lzw_code_t code;
	// lzw_code_t code = reader.readCode(this->bitWidth++);
	// lzw_code_t code = reader.getNextCode(this->bitWidth++);
	this->maxCodeValue = (1 << bitWidth) - 1;
	this->input.read(reinterpret_cast<char*>(&code), sizeof(lzw_code_t));
    std::vector<uint8_t> previousSequence = this->decompressionMap[code];

	// std::cout << "writing back ";
	// for (auto& c : previousSequence) {
	// 	std::bitset<8> temp = c;
	// 	std::cout << temp << " = " << temp.to_ulong() << std::endl;
	// }
	// std::cout << "\n";

	// std::cout << "writing back ";
	// std::cout << std::dec << (int) code << " || ";
	// for (auto& c : previousSequence) {
	// 	// std::bitset<8> temp = c;
	// 	// std::cout << temp << " = " << temp.to_ulong() << std::endl;
	// 	std::cout << (int) c << "-";
	// }
	// std::cout << "\n";
	this->target.write(reinterpret_cast<const char*>(previousSequence.data()), previousSequence.size());

	try {
		while (this->input.read(reinterpret_cast<char*>(&code), sizeof(lzw_code_t))) {
		// while (true) {
			// std::cout << input.tellg() << std::endl;
			// this->input.read(reinterpret_cast<char*>(&code), sizeof(lzw_code_t));
			// std::cout << "\nSending bitWidth " << this->bitWidth << std::endl;
			// code = reader.readCode(this->bitWidth);
			// code = reader.getNextCode(this->bitWidth);
			if (code > decompressionMap.size()) {
				std::cerr << "codigo lido estranho " << code << std::endl;
				return false;
			}
			this->decode(code, previousSequence, reader);
		}
	} catch (std::runtime_error& re) {
		std::cout << "Entrou no catch. Eu acho que terminou de ler o arquivo" << std::endl;
	}

	this->input.close();
	this->target.flush();
	this->target.close();

	return true;
}