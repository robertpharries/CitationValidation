#include "getApi.h"

using namespace std;


int main()
{

	doiIO input;
	input.read();
	for(int i = 0; i < input.count(); i++)
	{
		string doi = input.next();
		getFromApi(doi);
	}

	return 0;
}



