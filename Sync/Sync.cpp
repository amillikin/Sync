/********************************************************************************************
*										Sync.cpp 											*
*																							*
*	DESCRIPTION: Takes a wave file and attempts to detect steganographic properties			*
*				 Input Parameters: Sync <mode> <infile> <outfile>							*
*				 Modes: TE: Transmit(with errors)											*
*						TN: Transmit(no errors)												*
*						 R:	Receive															*
*																							*
*																							*
*	AUTHOR: Aaron Millikin											START DATE: 10/4/2017	*
*********************************************************************************************/
#include "stdafx.h"
#include <intrin.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <algorithm>
#include <cstdio>
#include <ctime>
#include <math.h>
#include <unordered_map>
using namespace std;

typedef const unordered_map<int, std::string> ASCIIStrMap_TRANS;
typedef const unordered_map<std::string, int> ASCIIStrMap_REC;

ifstream inFile;
ofstream outFile;

/*
	Constant Map for ASCII containing characters representing binary number WITH odd parity
	LITTLE endian
	Used during transmission.
*/
ASCIIStrMap_TRANS ascii_trans = {
	{   0, "00000001" }, // NUL (null)
	{   1, "10000000" }, // SOH (start of heading)
	{   2, "01000000" }, // STX (start of text)
	{   3, "11000001" }, // ETX (end of text)
	{   4, "00100000" }, // EOT (end of transmission)
	{   5, "10100001" }, // ENQ (enquiry)
	{   6, "01100001" }, // ACK (acknowledge)
	{   7, "11100000" }, // BEL (bell)
	{   8, "00010000" }, // BS (backspace)
	{   9, "10010001" }, // TAB (horizontal tab)
	{  10, "01010001" }, // LF (NL line feed, new line)
	{  11, "11010000" }, // VT (vertical tab)
	{  12, "00110001" }, // FF (NP form feed, new page)
	{  13, "10110000" }, // CR (carriage return)
	{  14, "01110000" }, // SO (shift out)
	{  15, "11110001" }, // SI (shift in)
	{  16, "00001000" }, // DLE (data link escape)
	{  17, "10001001" }, // DC1 (device control 1)
	{  18, "01001001" }, // DC2 (device control 2)
	{  19, "11001000" }, // DC3 (device control 3)
	{  20, "00101001" }, // DC4 (device control 4) 
	{  21, "10101000" }, // NAK (negative acknowledge)
	{  22, "01101000" }, // SYN (synchronous idle)
	{  23, "11101001" }, // ETB (end of trans. block)
	{  24, "00011001" }, // CAN (cancel)
	{  25, "10011000" }, // EM (end of medium)
	{  26, "01011000" }, // SUB (substitute)
	{  27, "11011001" }, // ESC (escape)
	{  28, "00111000" }, // FS (file separator)
	{  29, "10111001" }, // GS (group separator)
	{  30, "01111001" }, // RS (record separator)
	{  31, "11111000" }, // US (unit separator)
	{  32, "00000100" }, // ' '
	{  33, "10000101" }, // '!'
	{  34, "01000101" }, // '"'
	{  35, "11000100" }, // '#'
	{  36, "00100101" }, // '$'
	{  37, "10100100" }, // '%'
	{  38, "01100100" }, // '&'
	{  39, "11100101" }, // '''
	{  40, "00010101" }, // '('
	{  41, "10010100" }, // ')'
	{  42, "01010100" }, // '*'
	{  43, "11010101" }, // '+'
	{  44, "00110100" }, // ','
	{  45, "10110101" }, // '-'
	{  46, "01110101" }, // '.'
	{  47, "11110100" }, // '/'
	{  48, "00001101" }, // '0'
	{  49, "10001100" }, // '1'
	{  50, "01001100" }, // '2'
	{  51, "11001101" }, // '3'
	{  52, "00101100" }, // '4'
	{  53, "10101101" }, // '5'
	{  54, "01101101" }, // '6'
	{  55, "11101100" }, // '7'
	{  56, "00011100" }, // '8'
	{  57, "10011101" }, // '9'
	{  58, "01011101" }, // ':'
	{  59, "11011100" }, // ';'
	{  60, "00111101" }, // '<'
	{  61, "10111100" }, // '='
	{  62, "01111100" }, // '>'
	{  63, "11111101" }, // '?'
	{  64, "00000010" }, // '@'
	{  65, "10000011" }, // 'A'
	{  66, "01000011" }, // 'B'
	{  67, "11000010" }, // 'C'
	{  68, "00100011" }, // 'D'
	{  69, "10100010" }, // 'E'
	{  70, "01100010" }, // 'F'
	{  71, "11100011" }, // 'G'
	{  72, "00010011" }, // 'H'
	{  73, "10010010" }, // 'I'
	{  74, "01010010" }, // 'J'
	{  75, "11010011" }, // 'K'
	{  76, "00110010" }, // 'L'
	{  77, "10110011" }, // 'M'
	{  78, "01110011" }, // 'N'
	{  79, "11110010" }, // 'O'
	{  80, "00001011" }, // 'P'
	{  81, "10001010" }, // 'Q'
	{  82, "01001010" }, // 'R'
	{  83, "11001011" }, // 'S'
	{  84, "00101010" }, // 'T'
	{  85, "10101011" }, // 'U'
	{  86, "01101011" }, // 'V'
	{  87, "11101010" }, // 'W'
	{  88, "00011010" }, // 'X'
	{  89, "10011011" }, // 'Y'
	{  90, "01011011" }, // 'Z'
	{  91, "11011010" }, // '['
	{  92, "00111011" }, // '\'
	{  93, "10111010" }, // ']'
	{  94, "01111010" }, // '^'
	{  95, "11111011" }, // '_'
	{  96, "00000111" }, // '`'
	{  97, "10000110" }, // 'a'
	{  98, "01000110" }, // 'b'
	{  99, "11000111" }, // 'c'
	{ 100, "00100110" }, // 'd'
	{ 101, "10100111" }, // 'e'
	{ 102, "01100111" }, // 'f'
	{ 103, "11100110" }, // 'g'
	{ 104, "00010110" }, // 'h'
	{ 105, "10010111" }, // 'i'
	{ 106, "01010111" }, // 'j'
	{ 107, "11010110" }, // 'k'
	{ 108, "00110111" }, // 'l'
	{ 109, "10110110" }, // 'm'
	{ 110, "01110110" }, // 'n'
	{ 111, "11110111" }, // 'o'
	{ 112, "00001110" }, // 'p'
	{ 113, "10001111" }, // 'q'
	{ 114, "01001111" }, // 'r'
	{ 115, "11001110" }, // 's'
	{ 116, "00101111" }, // 't'
	{ 117, "10101110" }, // 'u'
	{ 118, "01101110" }, // 'v'
	{ 119, "11101111" }, // 'w'
	{ 120, "00011111" }, // 'x'
	{ 121, "10011110" }, // 'y'
	{ 122, "01011110" }, // 'z'
	{ 123, "11011111" }, // '{'
	{ 124, "00111110" }, // '|' 
	{ 125, "10111111" }, // '} 
	{ 126, "01111111" }, // '~'
	{ 127, "11111110" }  // DEL
};

/* 
	Constant Map for ASCII containing characters representing binary number WITH odd parity
	Used during receiving transmission. Failure to find a value will return TRANSMISSION ERROR.
*/
ASCIIStrMap_REC ascii_rec = {
	{ "00000001",   0 }, // NUL (null)
	{ "10000000",   1 }, // SOH (start of heading)
	{ "01000000",   2 }, // STX (start of text)
	{ "11000001",   3 }, // ETX (end of text)
	{ "00100000",   4 }, // EOT (end of transmission)
	{ "10100001",   5 }, // ENQ (enquiry)
	{ "01100001",   6 }, // ACK (acknowledge)
	{ "11100000",   7 }, // BEL (bell)
	{ "00010000",   8 }, // BS (backspace)
	{ "10010001",   9 }, // TAB (horizontal tab)
	{ "01010001",  10 }, // LF (NL line feed, new line)
	{ "11010000",  11 }, // VT (vertical tab)
	{ "00110001",  12 }, // FF (NP form feed, new page)
	{ "10110000",  13 }, // CR (carriage return)
	{ "01110000",  14 }, // SO (shift out)
	{ "11110001",  15 }, // SI (shift in)
	{ "00001000",  16 }, // DLE (data link escape)
	{ "10001001",  17 }, // DC1 (device control 1)
	{ "01001001",  18 }, // DC2 (device control 2)
	{ "11001000",  19 }, // DC3 (device control 3)
	{ "00101001",  20 }, // DC4 (device control 4) 
	{ "10101000",  21 }, // NAK (negative acknowledge)
	{ "01101000",  22 }, // SYN (synchronous idle)
	{ "11101001",  23 }, // ETB (end of trans. block)
	{ "00011001",  24 }, // CAN (cancel)
	{ "10011000",  25 }, // EM (end of medium)
	{ "01011000",  26 }, // SUB (substitute)
	{ "11011001",  27 }, // ESC (escape)
	{ "00111000",  28 }, // FS (file separator)
	{ "10111001",  29 }, // GS (group separator)
	{ "01111001",  30 }, // RS (record separator)
	{ "11111000",  31 }, // US (unit separator)
	{ "00000100",  32 }, // ' '
	{ "10000101",  33 }, // '!'
	{ "01000101",  34 }, // '"'
	{ "11000100",  35 }, // '#'
	{ "00100101",  36 }, // '$'
	{ "10100100",  37 }, // '%'
	{ "01100100",  38 }, // '&'
	{ "11100101",  39 }, // '''
	{ "00010101",  40 }, // '('
	{ "10010100",  41 }, // ')'
	{ "01010100",  42 }, // '*'
	{ "11010101",  43 }, // '+'
	{ "00110100",  44 }, // ','
	{ "10110101",  45 }, // '-'
	{ "01110101",  46 }, // '.'
	{ "11110100",  47 }, // '/'
	{ "00001101",  48 }, // '0'
	{ "10001100",  49 }, // '1'
	{ "01001100",  50 }, // '2'
	{ "11001101",  51 }, // '3'
	{ "00101100",  52 }, // '4'
	{ "10101101",  53 }, // '5'
	{ "01101101",  54 }, // '6'
	{ "11101100",  55 }, // '7'
	{ "00011100",  56 }, // '8'
	{ "10011101",  57 }, // '9'
	{ "01011101",  58 }, // ':'
	{ "11011100",  59 }, // ';'
	{ "00111101",  60 }, // '<'
	{ "10111100",  61 }, // '='
	{ "01111100",  62 }, // '>'
	{ "11111101",  63 }, // '?'
	{ "00000010",  64 }, // '@'
	{ "10000011",  65 }, // 'A'
	{ "01000011",  66 }, // 'B'
	{ "11000010",  67 }, // 'C'
	{ "00100011",  68 }, // 'D'
	{ "10100010",  69 }, // 'E'
	{ "01100010",  70 }, // 'F'
	{ "11100011",  71 }, // 'G'
	{ "00010011",  72 }, // 'H'
	{ "10010010",  73 }, // 'I'
	{ "01010010",  74 }, // 'J'
	{ "11010011",  75 }, // 'K'
	{ "00110010",  76 }, // 'L'
	{ "10110011",  77 }, // 'M'
	{ "01110011",  78 }, // 'N'
	{ "11110010",  79 }, // 'O'
	{ "00001011",  80 }, // 'P'
	{ "10001010",  81 }, // 'Q'
	{ "01001010",  82 }, // 'R'
	{ "11001011",  83 }, // 'S'
	{ "00101010",  84 }, // 'T'
	{ "10101011",  85 }, // 'U'
	{ "01101011",  86 }, // 'V'
	{ "11101010",  87 }, // 'W'
	{ "00011010",  88 }, // 'X'
	{ "10011011",  89 }, // 'Y'
	{ "01011011",  90 }, // 'Z'
	{ "11011010",  91 }, // '['
	{ "00111011",  92 }, // '\'
	{ "10111010",  93 }, // ']'
	{ "01111010",  94 }, // '^'
	{ "11111011",  95 }, // '_'
	{ "00000111",  96 }, // '`'
	{ "10000110",  97 }, // 'a'
	{ "01000110",  98 }, // 'b'
	{ "11000111",  99 }, // 'c'
	{ "00100110", 100 }, // 'd'
	{ "10100111", 101 }, // 'e'
	{ "01100111", 102 }, // 'f'
	{ "11100110", 103 }, // 'g'
	{ "00010110", 104 }, // 'h'
	{ "10010111", 105 }, // 'i'
	{ "01010111", 106 }, // 'j'
	{ "11010110", 107 }, // 'k'
	{ "00110111", 108 }, // 'l'
	{ "10110110", 109 }, // 'm'
	{ "01110110", 110 }, // 'n'
	{ "11110111", 111 }, // 'o'
	{ "00001110", 112 }, // 'p'
	{ "10001111", 113 }, // 'q'
	{ "01001111", 114 }, // 'r'
	{ "11001110", 115 }, // 's'
	{ "00101111", 116 }, // 't'
	{ "10101110", 117 }, // 'u'
	{ "01101110", 118 }, // 'v'
	{ "11101111", 119 }, // 'w'
	{ "00011111", 120 }, // 'x'
	{ "10011110", 121 }, // 'y'
	{ "01011110", 122 }, // 'z'
	{ "11011111", 123 }, // '{'
	{ "00111110", 124 }, // '|' 
	{ "10111111", 125 }, // '} 
	{ "01111111", 126 }, // '~'
	{ "11111110", 127 }  // DEL
};


// Simulates chance of transmission error
// Randomly generates a number 1-100. If rand# >= errChance
// Randomly changes one bit of the string being transmitted into either 0 or 1
string transError(string s, int errChance) {
	srand((unsigned int)time(NULL));

	if ((rand() % 100 + 1) <= errChance ) {
		s.at(rand() % 8) = (char) ((rand() % 2)+'0');
	}
	return s;
}

//Determines total number of 1 bits in a binary number. -ARM
int byteOnes(char byte) {
	int count;
	count = 0;
	while (byte > 0) {
		count++;
		byte &= (byte - 1);
	}
	return count;
}

//Test String Values from Map for Correctness
void checkStrings() {
	unsigned char c;
	int pcount;
	string s;
	c = 0;
	pcount = 0;
	for (int i = 0; i <= 127; i++) {
		s = ascii_trans.at(i);

		if (s.at(0) == '1') {
			c += 1;
			pcount++;
		}

		if (s.at(1) == '1') {
			c += 2;
			pcount++;
		}
		if (s.at(2) == '1') {
			c += 4;
			pcount++;
		}
		if (s.at(3) == '1') {
			c += 8;
			pcount++;
		}
		if (s.at(4) == '1') {
			c += 16;
			pcount++;
		}
		if (s.at(5) == '1') {
			c += 32;
			pcount++;

		}
		if (s.at(6) == '1') {
			c += 64;
			pcount++;
		}
		if (s.at(7) == '1') {
			//c += 1;
			pcount++;
		}

		outFile << transError(s, 50) << "(" << pcount << "): " << c << endl;
		c = 0;
		pcount = 0;
	}
}

//Converts a string to all uppercase characters - ARM
string upCase(string str) {
	transform(str.begin(), str.end(), str.begin(), toupper);
	return str;
}

//Checks valid mode - ARM
bool validMode(string mode) {
	if (mode == "TE" || mode == "TN" || mode == "R") {
		return true;
	}
	else {
		cout << "Not a valid mode. Valid modes include: TE, TN, R" << endl;
		return false;
	}
}

void prompt()
{
	cout << "Welcome to Aaron's Synchronous Transmitter/Receiver." << endl;
	cout << "Accepted input: Sync <mode> <infile> <outfile>" << endl;
	cout << "Modes: TE: Transmits(with errors), TN: Transmits(no errors), R: (Receives)" << endl;
}
int main(int argc, char* argv[]) {
	clock_t startTime = clock(), endTime;
	double secondsElapsed;
	string mode;
	char c;
	int cnt, fileSize;

	if (argc != 4) {
		cout << "Incorrect number of arguments supplied." << endl;
		prompt();
		return EXIT_FAILURE;
	}

	mode = upCase(argv[1]);
	if (!validMode(mode)) {
		prompt();
		return EXIT_FAILURE;
	}

	inFile.open(argv[2], ios::in);
	if (!inFile) {
		cout << "Can't open input file " << argv[1] << endl;
		prompt();
		return EXIT_FAILURE;
	}

	outFile.open(argv[3], ios::out);
	if (!outFile) {
		cout << "Can't open output file " << argv[2] << endl;
		prompt();
		return EXIT_FAILURE;
	}

	//Get Filesize
	fileSize = 0;
	inFile.seekg(0, ios::end);
	fileSize = inFile.tellg();
	inFile.seekg(0, ios::beg);

	checkStrings();
/*
	if (mode == "TN") {
		cnt = 0;
		// Sends binary representation (as '0' and '1' chars) of SYN twice. 
		outFile << ascii_trans.at(22) << ascii_trans.at(22);
		while (inFile.get(c)) {
			if (cnt == 63) {
				// New block, send SYN twice.
				outFile << ascii_trans.at(22) << ascii_trans.at(22);
				cnt = 0;
			}
			else cnt++;

			// Transmit char in binary (as '0' and '1' chars) with odd parity
			try {
				outFile << ascii_trans.at(c);
			}
			catch (int e) {
				cout << "An exception occurred. Exception Nr. " << e << endl;
			}
		}
	}
	else if (mode == "TE") {
		while (inFile.get(c)) {
			if (cnt == 63) {
				// New block, send SYN twice.
				outFile << ascii_trans.at(22) << ascii_trans.at(22);
				cnt = 0;
			}
			else cnt++;

			// Transmit char in binary (as '0' and '1' chars) with odd parity
			try {
				outFile << ascii_trans.at(c);
			}
			catch (int e) {
				cout << "An exception occurred. Exception Nr. " << e << endl;
			}

		}
	}
	else if (mode == "R") {

	}
*/
	inFile.close();
	outFile.close();

	endTime = clock();
	secondsElapsed = double(endTime - startTime) / CLOCKS_PER_SEC;
	cout << fixed << setprecision(3);
	cout << "Elapsed Time: " << secondsElapsed << endl;

	return EXIT_SUCCESS;
}