
#include "definitions.h"
#include "vcmdas.h"
#include "python_io.h"
#include <stdio.h>
#include <stdlib.h>

/** This is a little test program that is used to 
 * verify functionality of our inputs
 */

int main()
{
	VCMDAS* das = new VCMDAS();
	PythonIO* board = new PythonIO();
	das->setDigitalDirection(0, false);
	das->setDigitalDirection(1, false);
	board->initDigitalIO(0xFFFFFFFF);

	unsigned char byte0 = 0;
	unsigned char byte1 = 0;
	unsigned char byte2 = 0;
	unsigned char byte3 = 0;
	unsigned char tmp0 = 0;
	unsigned char tmp1 = 0;
	unsigned char tmp2 = 0;
	unsigned char tmp3 = 0;

	do{
		tmp0 = board->getDigital(0);
		tmp1 = board->getDigital(1);
		tmp2 = board->getDigital(2);
		tmp3 = board->getDigital(3);
		
		if(byte0 != tmp0 || byte1 != tmp1 || byte2 != tmp2 || byte3 != tmp3)
		{
			byte0 = tmp0;
			byte1 = tmp1;
			byte2 = tmp2;
			byte3 = tmp3;
			int i;
			for (i=0; i != 8; i++){
				if(bitof(i, byte0))
					printf("1");
				else
					printf("0");
			}
			printf(" ");
			for (i=0; i != 8; i++){
				if(bitof(i, byte1))
					printf("1");
				else
					printf("0");
			}
			printf(" ");
			for (i=0; i != 8; i++){
				if(bitof(i, byte2))
					printf("1");
				else
					printf("0");
			}
			printf(" ");
			for (i=0; i != 8; i++){
				if(bitof(i, byte3))
					printf("1");
				else
					printf("0");
			}
			printf("\n");
		}
			//printf("%c %c %c %c", byte0, byte1, byte2, byte3);

	}while(true);

	return 0;
};

