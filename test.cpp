
#include "definitions.h"
#include "vcmdas.h"
#include "python_io.h"
#include <stdio.h>
#include <stdlib.h>
int main()
{
	unsigned char foo = 0xF0;
	printf("FOO=%d\n", foo);
	bitset(2, &foo);
	printf("FOO=%d\n", foo);
	bitunset(2, &foo);
	printf("FOO=%d\n", foo);

	VCMDAS* das = new VCMDAS();
	PythonIO* board = new PythonIO();
	das->setDigitalDirection(0, false);
	das->setDigitalDirection(1, false);

	for(int j=0; j < 10000; j++)
	{

		for(unsigned char i=0; i<16; i++)
			double crap = das->getAnalog(i);

		for(unsigned char i=0; i<2; i++)
			das->setAnalog(i, 0);

		for(unsigned char i=0; i<4; i++)
			unsigned char crap = board->getDigital(i);

		for(unsigned char i=0; i<8; i++)
			unsigned short crap = board->getAnalog(i);

		board->setPWMDuty(22);
	}


	return 0;
};

