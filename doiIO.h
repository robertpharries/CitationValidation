#ifndef HEADER_DOIIO
#include <string>
#include <cstring>
#include <iterator>
#include <fstream>
#include <vector>
#include <iostream>

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
private:
	char* fname;
	vector<string> dois;
	unsigned int numDOI;
	bool customFile;
	vector<string>::iterator it;
};

#define HEADER_DOIIO
#endif