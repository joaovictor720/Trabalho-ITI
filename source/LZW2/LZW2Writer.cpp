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

void LZW2Writer::write(lzw_code_t code) {
	// output.write(reinterpret_cast<const char*>(&code), sizeof(code));
	// buf_size = code_width;
	//std::cout << "With code width " << code_width << std::endl;
	buf = (buf << code_width) | code;
	buf_size += code_width;
	while (buf_size >= BYTE_SIZE) {
		uint8_t byte_to_write = (buf >> (buf_size - BYTE_SIZE)) & 0xFF; // Pega os 8 bits mais antigos e válidos no buffer
		output.put(byte_to_write);
		buf_size -= BYTE_SIZE; // Descontando o byte que foi escrito
	}
}

void LZW2Writer::flush() {
	if (buf_size != 0) {
		uint8_t byte_to_write = (buf << buf_size - BYTE_SIZE) & 0xFF;
		output.put(byte_to_write);
		buf_size -= buf_size; // Buffer esvaziado
	}
}