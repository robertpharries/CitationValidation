/*
 *	Implementation of DOI citation validation IO module.
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

#include "doiIO.h"
#include "getApi.h"

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

}

/*
 *	Called to read the file for DOI's
 *	DOI's are stored in the module, they
 *	are accessed by calling getDoi()
 */
bool doiIO::read()
{
	this->ready = (true);
	ifstream ifs;
	ifs.exceptions ( ifstream::failbit | ifstream::badbit );

	try{

		ifs.open(this->fname);
		string line;
		while(ifs.good())
		{
			getline(ifs, line);
			if (line.size() > 4)
			{
				this->dois.push_back(line);
			}
		}
		ifs.close();

	} catch(std::ifstream::failure e) {
		cerr << "Error: Failed when opening/reading/closing file!" << endl;
		this->ready = (false);
		exit(0);
	}
	
	this->it = this->dois.begin();
	this->numDOI = this->dois.size();

	return this->ready;
}

/*
 *	Gives the i'th doi from the file
 *
 *	Requires read() to be successfully
 *	invoked.
 */
string doiIO::getDoi(int i)
{
	if (!(this->ready))
	{
		cerr << "Error: You cannot call next() until you have successfully called read(). Please call read first." << endl;
		string sent = "Error.";
		return sent;
	}

	return this->dois.at(i);
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
string doiIO::getFormattedFilename(int i)
{
	string newFname = this->dois.at(i);
	newFname = boost::replace_all_copy(newFname, "/", "_");
	return newFname + ".csv";
}

/*
 *	Public output interface.
 *
 *	Parameters:
 *		r (vector<ref*>)
 *		c (vector<corRef*>)
 */
bool doiIO::outputToCSV(int i, vector<ref*> r, vector<corRef*> c)
{
	ofstream ofs;

	try {
		if(r.size() > 0) {
			ofs.open(this->getFormattedFilename(i).c_str());

			ofs << "Title" << "," << "Authors" << "," << "Year"<< "," << "Source Title" << "," 
			<< "Volume No." << "," << "Issue No." << "," << "Start Page" << "," << "End Page" << "," << "DOI" 
			<< "," << "Status" << "," << " ," << "Title" << "," << "Authors" << "," << "Year" << "," 
			<< "Source Title" << "," << "Volume No." << "," << "Issue No." << "," << "Page Start" << "," 
			<< "End Page" << "," << "DOI" << "\n";

			string authorlist1;
			string authorlist2;

			for(int j = 0; j < r.size(); j++) {

				authorlist1 = "";
				authorlist2 = "";

				for (int k = 0; k < r.at(j)->authors.size(); k++)
				{
					authorlist1 = authorlist1 + r.at(j)->authors.at(k) + ", ";
				}

				for (int k = 0; k < c.at(j)->authors.size(); k++)
				{
					authorlist2 = authorlist2 + c.at(j)->authors.at(k) + ", ";
				}

				if (r.at(j)->authors.size() > 0)
				{
					authorlist1 = authorlist1.substr(0, authorlist1.size()-2);
				}

				if (c.at(j)->authors.size() > 0)
				{
					authorlist2 = authorlist2.substr(0, authorlist2.size()-2);
				}

				ofs << "\"" << r.at(j)->title << "\"" << ",";
				ofs << "\"" << authorlist1 << "\"" << ",";
				ofs << "\"" << r.at(j)->year << "\"" << ",";
				ofs << "\"" <<r.at(j)->sourceTitle << "\"" << ",";
				ofs << "\"" << r.at(j)->volume << "\"" << ",";
				ofs << "\"" << r.at(j)->issue << "\"" << ",";
				ofs << "\"" << r.at(j)->pageStart << "\"" << ",";
				ofs << "\"" << r.at(j)->pageEnd << "\"" << ",";
				ofs << "\"" << r.at(j)->doi << "\"" << ",";;
				ofs << "\"" << r.at(j)->status << "\"" << ",";
				ofs << " ,";
				ofs << "\"" << c.at(j)->title << "\"" << ",";
				ofs << "\"" << authorlist2 << "\"" << ",";
				ofs << "\"" << c.at(j)->year << "\"" << ","; 
				ofs << "\"" << c.at(j)->sourceTitle << "\"" << ",";
				ofs << "\"" << c.at(j)->volume << "\"" << ",";
				ofs << "\"" << c.at(j)->issue << "\"" << ",";
				ofs << "\"" << c.at(j)->pageStart << "\"" << ",";
				ofs << "\"" << c.at(j)->pageEnd << "\"" << ",";
				ofs << "\"" << c.at(j)->doi << "\"" << "\n";

			}

			cout << "Successfully written " << dois.at(i) << " to CSV file." << endl;

			ofs.close();
		}
	} catch(std::ofstream::failure e) {
		cerr << "Error: Failed when opening/writing/closing file!" << endl;
		return (false);
	}
	return (true);
}

/*
 *	Kick start function built to make use of
 *	API interface.
 */
void doiIO::perform()
{
	pthread_t threads[this->count()];
	for(int i = 0; i < this->count(); i++) {

		pthread_create(&threads[i], NULL, getFromApi, &this->dois.at(i));
	}

	result* currentResults[this->count()];

	for(int i = 0; i < this->count(); i++) {
		pthread_join(threads[i], (void**)&currentResults[i]);
	}

	for (int i = 0; i < this->count(); i++)
	{
		outputToCSV(i, currentResults[i]->initial, currentResults[i]->corrected);
	}
}
