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

/*
 *	Default Constructor.
 */
doiIO::doiIO()
{

	this->fname = new char[32];
	strncpy(this->fname, "doi.txt", 7);
	this->customFile = (false);
	this->ready = (false);
}

/*
 *	Constructor to manually specify a filename
 *	to extract DOI's from.
 *
 *	Parameters:
 *		a (char* with allocated memory)
 */
doiIO::doiIO(char* a)
{

	this->fname = a;
	this->customFile = (true);
	this->ready = (false);
}

/*
 *	Default Destructor.
 */
doiIO::~doiIO()
{

	delete[] this->fname;
}

/*
 *	Called to read the file for DOI's
 *	DOI's are stored in the module, they
 *	are accessed by calling next()
 */
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

/*
 *	Iterates through read in DOI's.
 *
 *	Requires read() to be successfully
 *	invoked.
 */
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

/*
 *	Returns the number of DOI values
 *	stored in the module.
 */
unsigned int doiIO::count()
{
	return this->numDOI;
}

/*
 *	Private function to construct filenames.
 */
string doiIO::getNextFormattedFilename()
{

	string newFname = boost::replace_all_copy(*outit++, "/", "_");
	return newFname + ".csv";
	
}

/*
 *	Public output interface.
 *
 *	Parameters:
 *		r (vector<ref*>)
 *		c (vector<corRef*>)
 */
bool doiIO::outputToCSV(vector<ref*> r, vector<corRef*> c)
{

	ofstream ofs;

	vector<ref*>::iterator i1;
	vector<corRef*>::iterator i2;



	try {

		ofs.open(this->getNextFormattedFilename().c_str());

		ofs << "Title" << "," << "Authors" << "," << "Year"<< "," << "Source Title" << "," 
		<< "Volume No." << "," << "Issue No." << "," << "Start Page" << "," << "End Page" << "," << "DOI" 
		<< "," << "Status" << "," << " ," << "Title" << "," << "Authors" << "," << "Year" << "," 
		<< "Source Title" << "," << "Volume No." << "," << "Issue No." << "," << "Page Start" << "," 
		<< "End Page" << "," << "DOI" << "\n";

		i1=r.begin();
		i2=c.begin();

		vector<string>::iterator si1 = (*i1)->authors.begin();
		vector<string>::iterator si2 = (*i2)->authors.begin();

		string authorlist1 = "";
		string authorlist2 = "";

		for( ; i1 < r.end(); i1++,i2++) {

			for (int i = 0; i < (*i1)->authors.size(); i++)
			{
				authorlist1 = authorlist1 + (*i1)->authors.at(i) + ", ";
			}

			for (int i = 0; i < (*i2)->authors.size(); i++)
			{
				authorlist2 = authorlist2 + (*i2)->authors.at(i) + ", ";
			}

			if ((*i1)->authors.size() > 0)
			{
				authorlist1 = authorlist1.substr(0, authorlist1.size()-2);
			}

			if ((*i2)->authors.size() > 0)
			{
				authorlist2 = authorlist2.substr(0, authorlist2.size()-2);
			}

			cerr << "Exit" << endl;

			ofs << "\"" << (*i1)->title << "\"" << "," << "\"" << authorlist1 << "\"" << "," << "\"" << (*i1)->year << "\"" << "," << "\"" << (*i1)->sourceTitle << "\"" << "," 
			<< "\"" << (*i1)->volume << "\"" << "," << "\"" << (*i1)->issue << "\"" << "," << "\"" << (*i1)->pageStart << "\"" << "," << "\"" << (*i1)->pageEnd << "\"" << "," << "\"" << (*i1)->doi 
			<< "\"" << "," << "\"" << (*i1)->status << "\"" << "," << "\"" << " ," << "\"" << (*i2)->title << "\"" << "," << "\"" << authorlist2 << "\"" << "," << "\"" << (*i2)->year << "," 
			<< "\"" << "," << "\"" << (*i1)->status << "\"" << "," << "\"" << " ," << "\"" << (*i2)->title << "\"" << "," << "\"" << authorlist2 << "\"" << "," << "\"" << (*i2)->year << "\"" << "," 
			<< "\"" << (*i2)->sourceTitle << "\"" << "," << "\"" << (*i2)->volume << "\"" << "," << "\"" << (*i2)->issue << "\"" << "," << "\"" << (*i2)->pageStart << "\"" << "," << "\""
			<< (*i2)->pageEnd << "\"" << "," << "\"" << (*i2)->doi << "\"" << "\n";
		}
		ofs.close();
	} catch(std::ofstream::failure e) {
		cerr << "Error: Failed when opening/writing/closing file!" << endl;
		return (false);
	}
	return (true);
}
