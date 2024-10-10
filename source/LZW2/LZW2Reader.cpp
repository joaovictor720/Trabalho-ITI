#include "LZW2Reader.h"

LZW2Reader::LZW2Reader(std::string& input)
{
	this->input = std::ifstream(input);
}

LZW2Reader::~LZW2Reader()
{

}

void LZW2Reader::close() {
	this->input.close();
}

long LZW2Reader::get_max_code_value() {
	return (1 << code_width) - 1;
}

void LZW2Reader::increment_width() {
	code_width++;
}

lzw_code_t LZW2Reader::read() {
	lzw_code_t code;
	end_of_file_signal = !input.read(reinterpret_cast<char*>(&code), sizeof(code));
	// end_of_file_signal = input.eof();
	return code;
}

bool LZW2Reader::eof() {
	return end_of_file_signal;
}