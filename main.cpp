#include "getApi.h"

using namespace std;


int main()
{

	doiIO doiTask;
	doiTask.read();
	for(int i = 0; i < doiTask.count(); i++)
	{
		doiTask.perform(i);
	}

	return 0;
}



