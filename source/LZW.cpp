#include "LZW.h"

LZW::LZW()
{

}

LZW::~LZW()
{

}

bool LZW::compress(std::string path) {
	std::ifstream input(path);
	std::ofstream target;

	this->initializeDictionary();

	u_int8_t byte;
	lastPrefix = "";
	while (true) {
		byte = input.get();
		if ((int) byte == EOF) {
			break;
		}
		std::string temp = lastPrefix + std::string(1, byte);

		if (dictionary[temp]) {
			lastPrefix = temp;
		} else {
			handleNewSymbol(byte);
		}
	}
	input.close();
	target.flush();
	target.close();
}