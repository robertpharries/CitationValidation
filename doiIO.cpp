/*
 *	Implementation of DOI citation validation IO module.
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

#include "doiIO.h"

using namespace std;

doiIO::doiIO()
{

	this->fname = new char[32];
	strncpy(this->fname, "doi.txt", 7);
	this->customFile = (false);
	this->ready = (false);
}

doiIO::doiIO(char* a)
{

	this->fname = a;
	this->customFile = (true);
	this->ready = (false);
}

doiIO::~doiIO()
{

	delete[] this->fname;
}

bool doiIO::read()
{

	this->ready = (true);
	ifstream ifs;

	try{

		ifs.open(this->fname);
		string line;
		while(ifs.good())
		{
			getline(ifs, line);
			this->dois.push_back(line);
		}
		ifs.close();

	} catch(std::ifstream::failure e) {
		cerr << "Error: Failed when opening/reading/closing file!" << endl;
		this->ready = (false);
		return ready;
	}
	
	this->it = this->dois.begin();
	this->outit = this->dois.begin();
	this->numDOI = this->dois.size();

	return this->ready;
}

string doiIO::next()
{

	if (!(this->ready))
	{
		cerr << "Error: You cannot call next() until you have successfully called read(). Please call read first." << endl;
		string sent = "Error.";
		return sent;
	}
	if (this->it >= this->dois.end())
	{
		string sent = "EOF";
		return sent;
	}

	return *it++;
}

unsigned int doiIO::count()
{
	return this->numDOI;
}

string doiIO::getNextFormattedFilename()
{

	if (this->outit >= this->dois.end())
	{
		string sent = "EOF";
		return sent;
	} else {
		string newFname = boost::replace_all_copy(*outit++, "/", "_");
		return newFname + ".csv";
	}
}

bool doiIO::outputToCSV(vector<ref*> r, vector<corRef*> c)
{

	ofstream ofs;
	vector<ref*>::iterator i1;
	vector<corRef*>::iterator i2;
	try {
		ofs.open(this->getNextFormattedFilename().c_str());
		i1=r.begin();
		i2=c.begin();
		for( ; i1 < r.end(); i1++,i2++) {
			ofs << (*i1)->title << "," << (*i1)->author << "," << (*i1)->year << "," << (*i1)->sourceTitle << "," 
			<< (*i1)->volume << "," << (*i1)->issue << "," << (*i1)->pageStart << "," << (*i1)->pageEnd << "," << (*i1)->doi 
			<< "," << (*i1)->status << "," << "00000000," << (*i2)->title << "," << (*i2)->author << "," << (*i2)->year << "," 
			<< (*i2)->sourceTitle << "," << (*i2)->volume << "," << (*i2)->issue << "," << (*i2)->pageStart << "," 
			<< (*i2)->pageEnd << "," << (*i2)->doi << "\n";
		}
		ofs.close();
	} catch(std::ifstream::failure e) {
		cerr << "Error: Failed when opening/writing/closing file!" << endl;
		return (false);
	}
	return (true);
}
