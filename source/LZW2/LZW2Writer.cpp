#include "LZW2Writer.h"

LZW2Writer::LZW2Writer(std::string& output)
{
	this->output = std::ofstream(output);
}

LZW2Writer::~LZW2Writer()
{

}

void LZW2Writer::close() {
	this->output.close();
}

long LZW2Writer::get_max_code_value() {
	return (1 << code_width) - 1;
}

void LZW2Writer::increment_width() {
	code_width++;
}

unsigned long long LZW2Writer::get_bits_written() {
	return bits_written;
}

void LZW2Writer::set_benchmarking(bool is_benchmarking) {
	this->is_benchmarking = is_benchmarking;
	if (is_benchmarking) {
		std::cout << "Benchmarking ATIVADO. Salvando dados em 'benchmark_data_file'." << std::endl;
	} else {
		std::cout << "Benchmarking DESATIVADO." << std::endl;
	}
}

void LZW2Writer::set_code_width_from_map_size(int size) {
	code_width = ceil(log2(size));
}

void LZW2Writer::write(lzw_code_t code) {
	buf = (buf << code_width) | code; // Inserindo os bits do código no buffer
	buf_size += code_width;
	while (buf_size >= BYTE_SIZE) {
		uint8_t byte_to_write = (buf >> (buf_size - BYTE_SIZE)) & 0xFF; // Pega os 8 bits mais antigos e válidos no buffer
		output.put(byte_to_write);
		bits_written += BYTE_SIZE;
		buf_size -= BYTE_SIZE; // Descontando o byte que foi escrito
	}
}

void LZW2Writer::flush() {
	if (buf_size != 0) {
		uint8_t byte_to_write = (buf << buf_size - BYTE_SIZE) & 0xFF;
		output.put(byte_to_write);
		bits_written += BYTE_SIZE;
		buf_size -= buf_size; // Buffer esvaziado
	}
}