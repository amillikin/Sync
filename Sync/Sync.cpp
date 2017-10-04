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
using namespace std;

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
	int fileSize;
	string mode;

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

	inFile.close();
	outFile.close();

	endTime = clock();
	secondsElapsed = double(endTime - startTime) / CLOCKS_PER_SEC;
	cout << fixed << setprecision(3);
	cout << "Elapsed Time: " << secondsElapsed << endl;

	return EXIT_SUCCESS;
}