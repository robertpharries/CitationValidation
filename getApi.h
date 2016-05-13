#include <iostream>
#include <string>
#include "doiIO.h"
//#include <curl/..>

//Alternatively, these could be stored as objects instead, with the output objects being included within them
struct ref{
	string title;
	string author;
	string year;
	string sourceTitle;
	string volume;
	string issue;
	string pageStart;
	string pageEnd;
	string doi;
	string status; //Possibly make it an enum, with options being ok, wrong, unsure or equivalent
};

struct corRef{
	string title;
	string author;
	string year;
	string sourceTitle;
	string volume;
	string issue;
	string pageStart;
	string pageEnd;
	string doi;
};

std::vector<ref*> getFromApi(string);