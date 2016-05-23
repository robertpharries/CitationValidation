/*
 *	Header of DOI citation validation IO module.
 *
 *	This module handles extracting DOI values from a local 
 *	file for processing, as well as outputting reference
 *	information to a .csv file.
 *
 *	Author:	Luke Mitchell, Robert Harries, Shannon Rothe, Brett Wilson
 *	Email:	lm671, rph289, smr999, baw246 @uowmail.edu.au
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
#include <pthread.h>

using namespace std;

struct ref{
	string title;
	vector<string> authors;
	string year;
	string sourceTitle;
	string volume;
	string issue;
	string pageStart;
	string pageEnd;
	string doi;
	string status; 
};

struct corRef{
	string title;
	vector<string> authors;
	string year;
	string sourceTitle;
	string volume;
	string issue;
	string pageStart;
	string pageEnd;
	string doi;
};

struct result {
	vector<ref*> initial;
	vector<corRef*> corrected;
};

struct prepComp {
	ref* initial;
	string metaid;
};

class doiIO
{
public:
	doiIO();
	doiIO(char*);
	~doiIO();
	bool read();
	string getDoi(int);
	unsigned int count();
	bool outputToCSV(int, vector<ref*>, vector<corRef*>);
	void perform();
private:
	char* fname;
	vector<string> dois;
	unsigned int numDOI;
	bool customFile;
	bool ready;
	vector<string>::iterator it;
	string getFormattedFilename(int);
};

#define HEADER_DOIIO
#endif