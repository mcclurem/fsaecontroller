
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
	board->initDigitalIO(0x00000000);

	unsigned char byte0 = 0;
	unsigned char byte1 = 0;
	unsigned char byte2 = 0;
	unsigned char byte3 = 0;
	unsigned char tmp0 = 0;
	unsigned char tmp1 = 0;
	unsigned char tmp2 = 0;
	unsigned char tmp3 = 0;


		board->setDigital(0, 0xFF);
		board->setDigital(1, 0x00);
		board->setDigital(2, 0xFF);
		board->setDigital(3, 0x00);
		board0 = board->getDigital(0);
		board1 = board->getDigital(1);
		board2 = board->getDigital(2);
		board3 = board->getDigital(3);
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
			//printf("%c %c %c %c", byte0, byte1, byte2, byte3);


	return 0;
};

