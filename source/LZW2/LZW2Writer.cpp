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

void LZW2Writer::write(lzw_code_t code) {
	output.write(reinterpret_cast<const char*>(&code), sizeof(code));
	// buf_size = code_width;
}