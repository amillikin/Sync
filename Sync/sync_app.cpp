/********************************************************************************************
*										Sync.cpp 											*
*																							*
*	DESCRIPTION: Simulates synchronous transmission and reception.							*
*				 Options to simulate transmissions errors or not.							*
*				 Input Parameters: Sync <mode> <infile> <outfile>							*
*				 Modes: TE: Transmit(with errors)											*
*						TN: Transmit(no errors)												*
*						 R:	Receive															*
*																							*
*																							*
*	AUTHOR: Aaron Millikin											START DATE: 10/4/2017	*
*********************************************************************************************/
#include "stdafx.h"
#include "sync_app.h"
#include "sync_phys.h"
using namespace std;

//Converts a string to all uppercase characters
string upCase(string str) {
	transform(str.begin(), str.end(), str.begin(), toupper);
	return str;
}

//Checks valid mode
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
	string mode;
	ifstream inFile;
	ofstream outFile;

	if (argc != 4) {
		cout << "Incorrect number of arguments supplied." << endl;
		prompt();
		return EXIT_FAILURE;
	}

	mode = upCase(argv[1]);
	if (!validMode(mode)) {
		cout << "Invalid mode.";
		prompt();
		return EXIT_FAILURE;
	}

	inFile.open(argv[2], ios::in | ios::binary);
	if (!inFile) {
		cout << "Can't open input file " << argv[1] << endl;
		prompt();
		return EXIT_FAILURE;
	}

	outFile.open(argv[3], ios::out | ios::binary);
	if (!outFile) {
		cout << "Can't open output file " << argv[2] << endl;
		prompt();
		return EXIT_FAILURE;
	}

	if (mode == "TN") {
		if (sync_phys::transWithOutErr(inFile, outFile) == 1) return EXIT_FAILURE;
	}
	else if (mode == "TE") {
		if (sync_phys::transWithErr(inFile, outFile) == 1) return EXIT_FAILURE;
	}
	else if (mode == "R") {
		if (sync_phys::receive(inFile, outFile) == 1) return EXIT_FAILURE;
	}

	inFile.close();
	outFile.close();

	return EXIT_SUCCESS;
}