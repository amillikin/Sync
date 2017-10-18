#include "stdafx.h"
#include "sync_data.h"

int sync_data::getFileSize(std::ifstream& inFile) {
	int fileSize = 0;

	inFile.seekg(0, std::ios::end);
	fileSize = inFile.tellg();
	inFile.seekg(0, std::ios::beg);

	return fileSize;
}

std::string sync_data::transError(std::string s, int errChance) {
	srand((unsigned int)time(NULL));

	if ((rand() % 100 + 1) <= errChance) {
		s.at(rand() % 8) = (char)((rand() % 2) + '0');
	}
	return s;
}