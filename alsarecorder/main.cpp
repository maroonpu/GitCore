#include <iostream>
#include "alsarec.h"

using namespace std;

int main(int argc, char const *argv[])
{
	AlsaRec myalsa;

	if(myalsa.recToWav("65.wav",5))
		cout<<"record error"<<endl;

	return 0;
}