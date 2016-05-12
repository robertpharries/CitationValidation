using namespace std;
#include "doiIO.h"
/*
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
	iterator it;
};
*/

doiIO::doiIO()
{

	this->fname = new char[32];
	strncpy(this->fname, "doi.txt", 7);
	this->customFile = (false);
}

doiIO::doiIO(char* a)
{

	this->fname = a;
	this->customFile = (true);
}

doiIO::~doiIO()
{

	delete[] this->fname;
}

bool doiIO::read()
{

	bool retval = (true);
	ifstream ifs;
	ifs.open(this->fname);
	string line;
	while(ifs.good())
	{
		getline(ifs, line);
		this->dois.push_back(line);
	}
	ifs.close();
	this->it = this->dois.begin();

	return retval;
}

string doiIO::next()
{
	if (this->it == this->dois.end())
	{
		string sent = "EOF";
		return sent;
	}

	return *it++;
}

unsigned int doiIO::count()
{
	return this->dois.size();
}
