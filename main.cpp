#include "getApi.h"

using namespace std;


int main(int argc, char const *argv[])
{
	char* filename;

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



