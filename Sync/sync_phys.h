#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <algorithm>

class sync_phys {
public:
	/*
	Transmits data from inFile to outFile with frames of
	64 character data blocks preceded by two SYN characters
	All characters transmitted are subject to transmission
	errors, at a hard coded 10% chance.
	*/
	static int transWithErr(std::ifstream& inFile, std::ofstream& outFile);

	/*
	Transmits data from inFile to outFile with frames of
	64 character data blocks preceded by two SYN characters
	*/
	static int transWithOutErr(std::ifstream& inFile, std::ofstream& outFile);

	/*
	Receives data from inFile, deconstructs each block,
	and outputs ASCII characters to outFile. Catches any
	parity errors that might have occured during transmission
	by checking the unordered map ASCII_REC, and upon not finding
	a corresponding entry, throws and error.
	*/
	static int receive(std::ifstream& inFile, std::ofstream& outFile);
};