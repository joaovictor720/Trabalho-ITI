#include "LZW2.h"

LZW2::LZW2()
{

}

LZW2::~LZW2()
{

}

void LZW2::set_restart_map_on_overflow(bool b) {
	this->restart_map_on_overflow = b;
}

void LZW2::set_max_sequences(long int max_sequences) {
	this->max_sequences = max_sequences;
}

void LZW2::initialize_maps() {
	this->compression_map.clear();
	this->decompression_map.clear();
	for (lzw_code_t i = 0; i < 256; i++) {
		this->compression_map[{static_cast<uint8_t>(i)}] = i;
		this->decompression_map[i] = {static_cast<uint8_t>(i)};
	}
}

void LZW2::compress(std::string& input_filename, std::string& output_filename) {
	std::ifstream input(input_filename, std::ios::binary);
	LZW2Writer writer(output_filename); // Responsável por escrever e contabilizar os bits no buffer

	// Iniciando os dicionários
	initialize_maps();
	
	// Passando por todos os bytes do arquivo
	std::vector<uint8_t> current_seq;
	char byte;
    while (input.get(byte)) {
		uint8_t symbol = static_cast<uint8_t>(byte);
		//std::cout << "Read original " << byte << std::endl;
		std::vector<uint8_t> new_seq = current_seq;
		new_seq.push_back(symbol); // Obtendo a sequência que contém o novo byte lido

		if (compression_map.find(new_seq) != compression_map.end()) {
			//std::cout << "Exists " << compression_map[new_seq] << "<>";
			for (auto& c : new_seq) {
				//std::cout << c << "-";
			}
			//std::cout << std::endl;
			// Se a nova sequência ainda estiver no dicionário, só continuar lendo a partir dela
			current_seq = new_seq;
		} else {
			//std::cout << "Doesn't exist ";
			for (auto& c : new_seq) {
				//std::cout << c << "-";
			}
			//std::cout << std::endl;
			// Se a nova sequência não estiver
			//std::cout << "Writing " << compression_map[current_seq] << "<>";
			for (auto& c : current_seq) {
				//std::cout << c << "-";
			}
			//std::cout << std::endl;
			writer.write(compression_map[current_seq]); // Mandando para saída o código da última sequência que estava no dicionário

			// Atualizando o dicionário (se possível)
			if (compression_map.size()+1 < max_sequences) {
				//std::cout << "Adding " << compression_map.size() << "<>";
				for (auto& c : new_seq) {
					//std::cout << c << "-";
				}
				//std::cout << std::endl << std::endl;
				
				// Se adicionar uma nova frase for necessitar de mais bits, incrementar a largura do código
				if (compression_map.size()+1 > writer.get_max_code_value()) {
					writer.increment_width();
				}
				compression_map[new_seq] = compression_map.size(); // Adicionando a nova frase
			} else {
				// Tratando o overflow do dicionário
				if (restart_map_on_overflow) {
					std::cout << "Reiniciando" << std::endl;
					initialize_maps();
				}
			}
			current_seq = {symbol}; // Reiniciando a sequência atual
		}
	}

	// Se o EOF for lido a última sequência lida ainda estava no dicionário, fazer flush dela na saída
	if (current_seq.size()) {
		//std::cout << "Writing (flush) " << compression_map[current_seq] << "<>";
		for (auto& c : current_seq) {
			//std::cout << c << "-";
		}
		//std::cout << std::endl;
		writer.write(compression_map[current_seq]);
		writer.flush();
	}

	input.close();
	writer.close();
}

void LZW2::decompress(std::string& input_filename, std::string& output_filename) {
	std::ofstream output(output_filename, std::ios::binary);
	LZW2Reader reader(input_filename);

	// Inicializando os dicionários
	initialize_maps();

	// Lendo e decodificando o primeiro código
	lzw_code_t code = reader.read();
	std::vector<uint8_t> previous_seq = decompression_map[code];
	
	// Na primeira leitura, já podemos considerar que a próxima frase já foi adicionada, só não está completa
	if (decompression_map.size()+1 > reader.get_max_code_value()) {
		reader.increment_width();
	}

	//std::cout << "Read back " << code << "<>";
	for (auto& c : previous_seq) {
		//std::cout << c << "-";
	}
	//std::cout << std::endl;

	//std::cout << "Writing ";
	for (auto& c : previous_seq) {
		//std::cout << c << "-";
	}
	//std::cout << std::endl;
	output.write(reinterpret_cast<const char*>(previous_seq.data()), previous_seq.size());

	while (true) {
		code = reader.read();
		if (reader.eof()) {
			break;
		}
		std::vector<uint8_t> current_seq; // Sequência associada ao código lido agora
		
		// Se o código existe no dicionário, só pegar a sequência associada a ele
		if (decompression_map.find(code) != decompression_map.end()) {
			current_seq = decompression_map[code];
		} else {
			// Se não, a nova sequência é a anterior junto com o primeiro dela mesma
			current_seq = previous_seq;
			current_seq.push_back(previous_seq[0]);
		}

		//std::cout << "Read back " << code << "<>";
		for (auto& c : current_seq) {
			//std::cout << c << "-";
		}
		//std::cout << std::endl;
		
		// Escrevendo na saída a sequência associada ao código lido
		//std::cout << "Writing ";
		for (auto& c : current_seq) {
			//std::cout << c << "-";
		}
		//std::cout << std::endl;
		output.write(reinterpret_cast<const char*>(current_seq.data()), current_seq.size());
		
		// Atualizando o dicionário (se possível)
		if (decompression_map.size()+1 < max_sequences) {
			// A nova sequência é a anterior + o símbolo que quebrou a anterior (primeiro da atual)
			std::vector<uint8_t> temp = previous_seq;
			temp.push_back(current_seq[0]);
			//std::cout << "Adding " << decompression_map.size() << "<>";
			for (auto& c : temp) {
				//std::cout << c << "-";
			}
			//std::cout << std::endl << std::endl;

			// Se adicionar uma nova frase for necessitar de mais bits, incrementar a largura do código
			if (decompression_map.size()+2 > reader.get_max_code_value()) {
				reader.increment_width();
			}
			decompression_map[decompression_map.size()] = temp;
		} else {
			// Tratando o overflow do dicionário
			if (restart_map_on_overflow) {
				initialize_maps();
			}
		}
		previous_seq = current_seq; // Atualizando a sequência anterior
	}


	output.flush();
	output.close();
	reader.close();
}