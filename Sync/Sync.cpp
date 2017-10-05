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

/*
	Constant Map for ASCII containing characters representing binary number WITH odd parity
	Used during transmission.
*/
ASCIIStrMap_TRANS ascii_trans = {
	{   0, "10000000" }, // NUL (null)
	{   1, "00000001" }, // SOH (start of heading)
	{   2, "10000010" }, // STX (start of text)
	{   3, "00000011" }, // ETX (end of text)
	{   4, "10000100" }, // EOT (end of transmission)
	{   5, "00000101" }, // ENQ (enquiry)
	{   6, "10000110" }, // ACK (acknowledge)
	{   7, "00000111" }, // BEL (bell)
	{   8, "10001000" }, // BS (backspace)
	{   9, "00001001" }, // TAB (horizontal tab)
	{  10, "10001010" }, // LF (NL line feed, new line)
	{  12, "00001011" }, // VT (vertical tab)
	{  13, "10001100" }, // FF (NP form feed, new page)
	{  14, "00001101" }, // CR (carriage return)
	{  15, "10001110" }, // SO (shift out)
	{  16, "00001111" }, // SI (shift in)
	{  17, "10010000" }, // DLE (data link escape)
	{  18, "00010001" }, // DC1 (device control 1)
	{  19, "10010010" }, // DC2 (device control 2)
	{  10, "00010011" }, // DC3 (device control 3)
	{  20, "10010100" }, // DC4 (device control 4) 
	{  21, "00010101" }, // NAK (negative acknowledge)
	{  22, "10010110" }, // SYN (synchronous idle)
	{  23, "00010111" }, // ETB (end of trans. block)
	{  24, "10011000" }, // CAN (cancel)
	{  25, "00011001" }, // EM (end of medium)
	{  26, "10011010" }, // SUB (substitute)
	{  27, "00011011" }, // ESC (escape)
	{  28, "10011100" }, // FS (file separator)
	{  29, "00011101" }, // GS (group separator)
	{  30, "10011110" }, // RS (record separator)
	{  31, "00011111" }, // US (unit separator)
	{  32, "10100000" }, // ' '
	{  33, "00100001" }, // '!'
	{  34, "10100010" }, // '"'
	{  35, "00100011" }, // '#'
	{  36, "10100100" }, // '$'
	{  37, "00100101" }, // '%'
	{  38, "10100110" }, // '&'
	{  39, "00100111" }, // '''
	{  40, "10101000" }, // '('
	{  41, "00101001" }, // ')'
	{  42, "10101010" }, // '*'
	{  43, "00101011" }, // '+'
	{  44, "10101100" }, // ','
	{  45, "00101101" }, // '-'
	{  46, "10101110" }, // '.'
	{  47, "00101111" }, // '/'
	{  48, "10110000" }, // '0'
	{  49, "00110001" }, // '1'
	{  50, "10110010" }, // '2'
	{  51, "00110011" }, // '3'
	{  52, "10110100" }, // '4'
	{  53, "00110101" }, // '5'
	{  54, "10110110" }, // '6'
	{  55, "00110111" }, // '7'
	{  56, "10111000" }, // '8'
	{  57, "00111001" }, // '9'
	{  58, "10111010" }, // ':'
	{  59, "00111011" }, // ';'
	{  60, "10111100" }, // '<'
	{  61, "00111101" }, // '='
	{  62, "10111110" }, // '>'
	{  63, "00111111" }, // '?'
	{  64, "11000000" }, // '@'
	{  65, "01000001" }, // 'A'
	{  66, "11000010" }, // 'B'
	{  67, "01000011" }, // 'C'
	{  68, "11000100" }, // 'D'
	{  69, "01000101" }, // 'E'
	{  70, "11000110" }, // 'F'
	{  71, "01000111" }, // 'G'
	{  72, "11001000" }, // 'H'
	{  73, "01001001" }, // 'I'
	{  74, "11001010" }, // 'J'
	{  75, "01001011" }, // 'K'
	{  76, "11001100" }, // 'L'
	{  77, "01001101" }, // 'M'
	{  78, "11001110" }, // 'N'
	{  79, "01001111" }, // 'O'
	{  80, "11010000" }, // 'P'
	{  81, "01010001" }, // 'Q'
	{  82, "11010010" }, // 'R'
	{  83, "01010011" }, // 'S'
	{  84, "11010100" }, // 'T'
	{  85, "01010101" }, // 'U'
	{  86, "11010110" }, // 'V'
	{  87, "01010111" }, // 'W'
	{  88, "11011000" }, // 'X'
	{  89, "01011001" }, // 'Y'
	{  90, "11011010" }, // 'Z'
	{  91, "01011011" }, // '['
	{  92, "11011100" }, // '\'
	{  93, "01011101" }, // ']'
	{  94, "11011110" }, // '^'
	{  95, "01011111" }, // '_'
	{  96, "11100000" }, // '`'
	{  97, "01100001" }, // 'a'
	{  98, "11100010" }, // 'b'
	{  99, "01100011" }, // 'c'
	{ 100, "11100100" }, // 'd'
	{ 101, "01100101" }, // 'e'
	{ 102, "11100110" }, // 'f'
	{ 103, "01100111" }, // 'g'
	{ 104, "11101000" }, // 'h'
	{ 105, "01101001" }, // 'i'
	{ 106, "11101010" }, // 'j'
	{ 107, "01101011" }, // 'k'
	{ 108, "11101100" }, // 'l'
	{ 109, "01101101" }, // 'm'
	{ 110, "11101110" }, // 'n'
	{ 111, "01101111" }, // 'o'
	{ 112, "11110000" }, // 'p'
	{ 113, "01110001" }, // 'q'
	{ 114, "11110010" }, // 'r'
	{ 115, "01110011" }, // 's'
	{ 116, "11110100" }, // 't'
	{ 117, "01110101" }, // 'u'
	{ 118, "11110110" }, // 'v'
	{ 119, "01110111" }, // 'w'
	{ 120, "11111000" }, // 'x'
	{ 121, "01111001" }, // 'y'
	{ 122, "11111010" }, // 'z'
	{ 123, "01111011" }, // '{'
	{ 124, "11111100" }, // '|' 
	{ 125, "01111101" }, // '} 
	{ 126, "11111110" }, // '~'
	{ 127, "01111111" }  // DEL
};

/* 
	Constant Map for ASCII containing characters representing binary number WITH odd parity
	Used during receiving transmission. Failure to find a value will return TRANSMISSION ERROR.
*/
ASCIIStrMap_REC ascii_rec = {
	{ "10000000", 0   }, // NUL (null)
	{ "00000001", 1   }, // SOH (start of heading)
	{ "10000010", 2   }, // STX (start of text)
	{ "00000011", 3   }, // ETX (end of text)
	{ "10000100", 4   }, // EOT (end of transmission)
	{ "00000101", 5   }, // ENQ (enquiry)
	{ "10000110", 6   }, // ACK (acknowledge)
	{ "00000111", 7   }, // BEL (bell)
	{ "10001000", 8   }, // BS (backspace)
	{ "00001001", 9   }, // TAB (horizontal tab)
	{ "10001010", 10  }, // LF (NL line feed, new line)
	{ "00001011", 11  }, // VT (vertical tab)
	{ "10001100", 12  }, // FF (NP form feed, new page)
	{ "00001101", 13  }, // CR (carriage return)
	{ "10001110", 14  }, // SO (shift out)
	{ "00001111", 15  }, // SI (shift in)
	{ "10010000", 16  }, // DLE (data link escape)
	{ "00010001", 17  }, // DC1 (device control 1)
	{ "10010010", 18  }, // DC2 (device control 2)
	{ "00010011", 19  }, // DC3 (device control 3)
	{ "10010100", 20  }, // DC4 (device control 4) 
	{ "00010101", 21  }, // NAK (negative acknowledge)
	{ "10010110", 22  }, // SYN (synchronous idle)
	{ "00010111", 23  }, // ETB (end of trans. block)
	{ "10011000", 24  }, // CAN (cancel)
	{ "00011001", 25  }, // EM (end of medium)
	{ "10011010", 26  }, // SUB (substitute)
	{ "00011011", 27  }, // ESC (escape)
	{ "10011100", 28  }, // FS (file separator)
	{ "00011101", 29  }, // GS (group separator)
	{ "10011110", 30  }, // RS (record separator)
	{ "00011111", 31  }, // US (unit separator)
	{ "10100000", 32  }, // ' '
	{ "00100001", 33  }, // '!'
	{ "10100010", 34  }, // '"'
	{ "00100011", 35  }, // '#'
	{ "10100100", 36  }, // '$'
	{ "00100101", 37  }, // '%'
	{ "10100110", 38  }, // '&'
	{ "00100111", 39  }, // '''
	{ "10101000", 40  }, // '('
	{ "00101001", 41  }, // ')'
	{ "10101010", 42  }, // '*'
	{ "00101011", 43  }, // '+'
	{ "10101100", 44  }, // ','
	{ "00101101", 45  }, // '-'
	{ "10101110", 46  }, // '.'
	{ "00101111", 47  }, // '/'
	{ "10110000", 48  }, // '0'
	{ "00110001", 49  }, // '1'
	{ "10110010", 50  }, // '2'
	{ "00110011", 51  }, // '3'
	{ "10110100", 52  }, // '4'
	{ "00110101", 53  }, // '5'
	{ "10110110", 54  }, // '6'
	{ "00110111", 55  }, // '7'
	{ "10111000", 56  }, // '8'
	{ "00111001", 57  }, // '9'
	{ "10111010", 58  }, // ':'
	{ "00111011", 59  }, // ';'
	{ "10111100", 60  }, // '<'
	{ "00111101", 61  }, // '='
	{ "10111110", 62  }, // '>'
	{ "00111111", 63  }, // '?'
	{ "11000000", 64  }, // '@'
	{ "01000001", 65  }, // 'A'
	{ "11000010", 66  }, // 'B'
	{ "01000011", 67  }, // 'C'
	{ "11000100", 68  }, // 'D'
	{ "01000101", 69  }, // 'E'
	{ "11000110", 70  }, // 'F'
	{ "01000111", 71  }, // 'G'
	{ "11001000", 72  }, // 'H'
	{ "01001001", 73  }, // 'I'
	{ "11001010", 74  }, // 'J'
	{ "01001011", 75  }, // 'K'
	{ "11001100", 76  }, // 'L'
	{ "01001101", 77  }, // 'M'
	{ "11001110", 78  }, // 'N'
	{ "01001111", 79  }, // 'O'
	{ "11010000", 80  }, // 'P'
	{ "01010001", 81  }, // 'Q'
	{ "11010010", 82  }, // 'R'
	{ "01010011", 83  }, // 'S'
	{ "11010100", 84  }, // 'T'
	{ "01010101", 85  }, // 'U'
	{ "11010110", 86  }, // 'V'
	{ "01010111", 87  }, // 'W'
	{ "11011000", 88  }, // 'X'
	{ "01011001", 89  }, // 'Y'
	{ "11011010", 90  }, // 'Z'
	{ "01011011", 91  }, // '['
	{ "11011100", 92  }, // '\'
	{ "01011101", 93  }, // ']'
	{ "11011110", 94  }, // '^'
	{ "01011111", 95  }, // '_'
	{ "11100000", 96  }, // '`'
	{ "01100001", 97  }, // 'a'
	{ "11100010", 98  }, // 'b'
	{ "01100011", 99  }, // 'c'
	{ "11100100", 100 }, // 'd'
	{ "01100101", 101 }, // 'e'
	{ "11100110", 102 }, // 'f'
	{ "01100111", 103 }, // 'g'
	{ "11101000", 104 }, // 'h'
	{ "01101001", 105 }, // 'i'
	{ "11101010", 106 }, // 'j'
	{ "01101011", 107 }, // 'k'
	{ "11101100", 108 }, // 'l'
	{ "01101101", 109 }, // 'm'
	{ "11101110", 110 }, // 'n'
	{ "01101111", 111 }, // 'o'
	{ "11110000", 112 }, // 'p'
	{ "01110001", 113 }, // 'q'
	{ "11110010", 114 }, // 'r'
	{ "01110011", 115 }, // 's'
	{ "11110100", 116 }, // 't'
	{ "01110101", 117 }, // 'u'
	{ "11110110", 118 }, // 'v'
	{ "01110111", 119 }, // 'w'
	{ "11111000", 120 }, // 'x'
	{ "01111001", 121 }, // 'y'
	{ "11111010", 122 }, // 'z'
	{ "01111011", 123 }, // '{'
	{ "11111100", 124 }, // '|' 
	{ "01111101", 125 }, // '} 
	{ "11111110", 126 }, // '~'
	{ "01111111", 127 }  // DEL
};

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
	ifstream inFile;
	ofstream outFile;
	string mode;
	char c;
	int cnt;

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

	// Sends binary representation (as '0' and '1' chars) of SYN twice. 
	outFile << ascii_trans.at(22) << ascii_trans.at(22);

	cnt = 0;
	while (inFile.get(c)) {
		if (cnt == 63) {
			// New block, send SYN twice.
			outFile << ascii_trans.at(22) << ascii_trans.at(22);
			cnt = 0;
		}
		else cnt++;

		// Transmit char in binary (as '0' and '1' chars) with odd parity
		outFile << ascii_trans.at(c);
	}

	inFile.close();
	outFile.close();

	endTime = clock();
	secondsElapsed = double(endTime - startTime) / CLOCKS_PER_SEC;
	cout << fixed << setprecision(3);
	cout << "Elapsed Time: " << secondsElapsed << endl;

	return EXIT_SUCCESS;
}