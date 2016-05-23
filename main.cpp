/*
 *	Driver for the Citation Verification Tool
 *
 *	This program takes Doi's from file, retrieves the online citation 
 *	records for the Doi, and checks them against the citation given by
 * 	the cited article/paper. It will then output result to CSV, allowing
 * 	for user review.
 *
 *	Author:	Luke Mitchell, Robert Harries, Shannon Rothe, Brett Wilson
 *	Email:	lm671, rph289, smr999, baw246 @uowmail.edu.au
 *	Github:	http://github.com/auVeypor/311
 *	Group:	Project #2, Group #1
 * 
 */

#include "getApi.h"
#include "doiIO.h"

using namespace std;

/*
 *	Driver function. Reads the command line input and 
 *	runs the functionality appropriately
 */
int main(int argc, char const *argv[])
{
	char* filename;
	//check command line input. If file name specified, use it, else, 
	//it defaults to doi.txt
	if(argc > 2) {
		cout << "Usage: " << argv[0] << " <filename>" << endl; 
		return 0;
	} else if(argc > 1) {
		filename = new char[128];
		strncpy(filename, argv[1], 127);
		doiIO doiTask(filename);
		doiTask.read();
		doiTask.perform();
	} else {
		doiIO doiTask;
		doiTask.read();
		doiTask.perform();
	}


	return 0;
}



