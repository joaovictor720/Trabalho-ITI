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

void LZW2Reader::set_code_width_from_map_size(int size) {
	code_width = ceil(log2(size));
}

lzw_code_t LZW2Reader::read() {
	lzw_code_t code;
	
	while (buf_size < code_width) {
		char byte;
		end_of_file_signal = !input.get(byte);
		buf = (buf << BYTE_SIZE) | (uint8_t) byte;
		buf_size += BYTE_SIZE;
	}
	uint64_t dynamic_mask = ((1 << code_width) - 1);
	code = (buf >> (buf_size - code_width)) & dynamic_mask;
	buf_size -= code_width;

	return code;
}

bool LZW2Reader::eof() {
	return end_of_file_signal;
}