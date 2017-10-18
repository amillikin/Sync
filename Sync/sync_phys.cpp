#include "stdafx.h"
#include "sync_phys.h"
#include "sync_data.h"
using namespace std;

const std::unordered_map<int, std::string> sync_data::ascii_map::ascii_trans = sync_data::ascii_map::createTransMap();
const std::unordered_map<std::string, int> sync_data::ascii_map::ascii_rec = sync_data::ascii_map::createRecMap();

int sync_phys::transWithErr(ifstream& inFile, ofstream& outFile) {
	int fileSize, frameCnt, dataBytesLeft;
	const int errChance = 10;
	char c = 0;

	//Get Filesize
	fileSize = sync_data::getFileSize(inFile);

	//Break filesize into 64 byte data blocks
	frameCnt = fileSize / 64;

	//Determine size, if any, of data leftover after full frames
	dataBytesLeft = (fileSize % 64);

	//Read all full frames
	for (int i = 0; i < frameCnt; i++) {
		// New block, send SYN twice, length 64.
		outFile << sync_data::transError(sync_data::ascii_map::ascii_trans.at(22), errChance)
			<< sync_data::transError(sync_data::ascii_map::ascii_trans.at(22), errChance)
			<< sync_data::transError(sync_data::ascii_map::ascii_trans.at(64), errChance);

		for (int j = 0; j < 64; j++) {
			// Transmit char in binary (as '0' and '1' chars) with odd parity
			inFile.get(c);
			outFile << sync_data::transError(sync_data::ascii_map::ascii_trans.at(c), errChance);
		}
	}

	//If there are data bytes left after the last full frame
	//Write the SYN characters, the size, and then
	//Transmit each char in binary (as '0' and '1' chars) with odd parity
	if (dataBytesLeft > 0) {
		outFile << sync_data::transError(sync_data::ascii_map::ascii_trans.at(22), errChance)
			<< sync_data::transError(sync_data::ascii_map::ascii_trans.at(22), errChance)
			<< sync_data::transError(sync_data::ascii_map::ascii_trans.at(dataBytesLeft), errChance);
		while (inFile.get(c)) {
			outFile << sync_data::transError(sync_data::ascii_map::ascii_trans.at(c), errChance);
		}
	}
	return 0;
}

int sync_phys::transWithOutErr(ifstream& inFile, ofstream& outFile) {
	int fileSize, frameCnt, dataBytesLeft;
	char c = 0;

	//Get Filesize
	fileSize = sync_data::getFileSize(inFile);

	//Break filesize into 64 byte data blocks
	frameCnt = fileSize / 64;

	//Determine size, if any, of data leftover after full frames
	dataBytesLeft = (fileSize % 64);

	//Read all full frames
	for (int i = 0; i < frameCnt; i++) {
		// New block, send SYN twice, length 64.
		outFile << "011010000110100000000010"; //CHR(22)+CHR(22)+CHR(64)
		for (int j = 0; j < 64; j++) {
			// Transmit char in binary (as '0' and '1' chars) with odd parity
			inFile.get(c);
			outFile << sync_data::ascii_map::ascii_trans.at(c);
		}
	}

	//If there are data bytes left after the last full frame
	//Write the SYN characters, the size, and then
	//Transmit each char in binary (as '0' and '1' chars) with odd parity
	if (dataBytesLeft > 0) {
		outFile << "0110100001101000" << sync_data::ascii_map::ascii_trans.at(dataBytesLeft); //CHR(22)+CHR(22)+DATA SIZE CHAR
		for (int i = 0; i < dataBytesLeft; i++) {
			inFile.get(c);
			outFile << sync_data::ascii_map::ascii_trans.at(c);
		}
	}
	return 0;
}

int sync_phys::receive(ifstream& inFile, ofstream& outFile) {
	int fileSize, frameCnt, dataSize;
	char dataChar[8];
	const string synChars = "0110100001101000";
	sync_data::frameHeader fh;

	//Get Filesize
	fileSize = sync_data::getFileSize(inFile);

	//Break filesize into each full frame
	//16 chars (SYN SYN) + 8 chars (length char) + 512 (64*8) data chars = 536 chars
	frameCnt = fileSize / 536;

	//Determine size, if any, of data leftover after full frames
	//Get remainder of division, subtract 16 bytes of SYN SYN, 8 bytes of length char
	//dataBytesLeft = (fileSize % 536) - 24;

	//Determines if there is a frame with less than 64 data characters, if so, add one to frameCnt
	if (fileSize % 536 > 0) frameCnt++;

	//Read all full frames
	//Check SYN as first two chars
	for (int i = 0; i < frameCnt; i++) {
		inFile.read((char *)&fh, sizeof(fh));
		string strSyn(fh.synChars, fh.synChars + 16);
		if (synChars != strSyn) {
			cout << "SYN character error detected." << endl;
			return 1;
		}

		string strLen(fh.lenChar, fh.lenChar + 8);

		if (sync_data::ascii_map::ascii_rec.find(strLen) != sync_data::ascii_map::ascii_rec.end()) {
			dataSize = sync_data::ascii_map::ascii_rec.at(strLen);
		}
		else {
			cout << "Length character error detected." << endl;
			return 1;
		}

		for (int j = 0; j < dataSize; j++) {
			inFile.read((char *)&dataChar, sizeof(dataChar));
			string strData(dataChar, dataChar + 8);
			if (sync_data::ascii_map::ascii_rec.find(strData) != sync_data::ascii_map::ascii_rec.end()) {
				outFile << (char)sync_data::ascii_map::ascii_rec.at(strData);
			}
			else {
				cout << "Data character error detected." << endl;
				return 1;
			}
		}
	}

	return 0;
}