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
 *
 *		USAGE:
 *			1.	Instantiate with doiIO() or doiIO(char*) (if you want a custom filename)
 *			2.	Invoke read()
 *			3.	Extract your data with next(). next() will work count() times. next() 
 *				will return "EOF" if you call past that value.
 *			4.	Do stuff with your data.
 *			5.	When you are done, for each DOI/output file, put your references in a
 *				vector<ref*> and your corrected references in vector<corRef*>, and pass 
 *				them to outputToCSV().
 *			6.	Done!
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