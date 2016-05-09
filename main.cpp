#include <iostream>
#include <string>
#include "doiIO.h"
//#include <curl/..>

using namespace std;

//Alternatively, these could be stored as objects instead, with the output objects being included within them
struct reference{
	string title;
	string author;
	int year;
	string sourceTitle;
	int volume;
	int issue;
	int pageStart;
	int pageEnd;
	string doi;
	string status; //Possibly make it an enum, with options being ok, wrong, unsure or equivalent
};

struct correctedRef{
	string title;
	string author;
	int year;
	string sourceTitle;
	int volume;
	int issue;
	int pageStart;
	int pageEnd;
	string doi;
};

int main()
{
	//read DOI input from file using doiIO
	doiIO input;
	//Create reference structs as a vector/array based on input.count()

	for(int i = 0; i < input.count(); i++)
	{
		string doi = input.next();

		//query API -> scopus
		//process the resulting API json
		//Place into an array/vector of structs
		bool processed = false;
		while(!processed)
		{
			correctedRef corrRef;
			//Call process api object, pass it the reference struct
			//Check for a match in scopus API until a match is found or not match can be identified (Process Results Class)
			//Compare returned DOI and add corrected references in the correctedRef struct
		}

		//Call output function to create the csv file and output the reference struct and corrRef
		//Will need to be passed both structs according to the current input.count()
	}




	return 0;
}



