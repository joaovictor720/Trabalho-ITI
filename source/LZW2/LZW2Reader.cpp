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