#include <cstring>
#include <iostream>
#include "LZW2.h"

void print_usage(const std::string& programName) {
    std::cerr << "Uso: " << programName << " <-c/-d> <arquivo de input> <arquivo de output> "
              << "[-m <máximo de bytes no treinamento do modelo>] "
              << "[-sm <salvar modelo>] "
              << "[-um <usar modelo específico> <arquivo do modelo>]" << std::endl;
}

int main(int argc, char** argv) {
	bool restart_map_on_overflow = false;
	bool compress = false;
	bool decompress = false;
	bool is_using_model = false;
	bool save_model = false;
	unsigned long long max_map_phrases = (1 << sizeof(lzw_code_t)*8) - 1;
	std::string input = "";
	std::string output = "";
	std::string model_file = "";

	if (argc > 0) {
		for (int i = 0; i < argc; i++) {
			if (!strcmp("-m", argv[i]) && i+1 < argc) {
				max_map_phrases = std::stoull(argv[++i]);
			} else if (!strcmp("-r", argv[i])) {
				restart_map_on_overflow = true;
			} else if (!strcmp("-c", argv[i]) && i+2 < argc) {
				compress = true;
				input = argv[++i];
				output = argv[++i];
			} else if (!strcmp("-d", argv[i]) && i+2 < argc) {
				decompress = true;
				input = argv[++i];
				output = argv[++i];
			} else if (!strcmp("-um", argv[i]) && i+1 < argc) {
				is_using_model = true;
				model_file = argv[++i];
			} else if (!strcmp("-sm", argv[i]) && i+1 < argc) {
				save_model = true;
				model_file = argv[i+1];
			}
		}
	}

	if (input == "" || output == "" || input == output) {
		std::cerr << "Erro: Arquivo de input e output vazios ou iguais." << std::endl;
		return 1;
	}
	if (is_using_model && model_file == "") {
		std::cerr << "Erro: Nome do arquivo do modelo vazio." << std::endl;
	}
	
	LZW2 lzw2;
	lzw2.set_max_sequences(max_map_phrases);
	lzw2.set_restart_map_on_overflow(restart_map_on_overflow);
	if (is_using_model) {
		lzw2.load_model(model_file);
	}

	// Printando configurações
	std::cout << "Número de frases no dicionário: " << max_map_phrases << std::endl;
	if (restart_map_on_overflow) {
		std::cout << "Estratégia de overflow: REINICIAR" << std::endl;
	} else {
		std::cout << "Estratégia de overflow: ESTÁTICO" << std::endl;
	}
	if (is_using_model) {
		std::cout << "Usando modelo: " << model_file << std::endl;
	} else {
		std::cout << "Usando modelo: <nenhum modelo>" << std::endl;
	}

	if (compress) {
		std::cout << "Comprimindo '" << input << "' para '" << output << "'" << std::endl;
		lzw2.compress(input, output);
		if (save_model) {
			lzw2.save_model(model_file);
		}
	} else if (decompress) {
		std::cout << "Descomprimindo '" << input << "' para '" << output << "'" << std::endl;
		lzw2.decompress(input, output);
	} else {
		std::cout << "Num vai nem comprimir nem descomprimir, vai fazer oq??" << std::endl;
		print_usage(argv[0]);
	}
	
	return 0;
}