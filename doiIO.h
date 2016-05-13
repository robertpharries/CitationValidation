/*
 *	Header for DOI citation validation IO module.
 *
 *	This module handles extracting DOI values from a local 
 *	file for processing, as well as outputting reference
 *	information to a .csv file.
 *
 *	Author:	Luke Mitchell
 *	Email:	lm671@uowmail.edu.au
 *	Github:	http://github.com/auVeypor/311
 *	Group:	Project #2, Group #1
 * 
 */

#ifndef HEADER_DOIIO
#include <string>
#include <cstring>
#include <iterator>
#include <fstream>
#include <vector>
#include <iostream>
#include <boost/algorithm/string/replace.hpp>

using namespace std;

class doiIO
{
public:
	doiIO();
	doiIO(char*);
	~doiIO();
	bool read();
	string next();
	unsigned int count();
	bool outputToCSV(vector<ref*>, vector<corRef*>);
private:
	char* fname;
	vector<string> dois;
	unsigned int numDOI;
	bool customFile;
	bool ready;
	vector<string>::iterator it;
	vector<string>::iterator outit;
	string getNextFormattedFilename();
};

#define HEADER_DOIIO
#endif