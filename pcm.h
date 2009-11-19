
#include <sys/io.h>


#ifndef PCM_H
#define PCM_H

class PCM
{
	public:
		PCM(unsigned short base_address);
		unsigned char getDigital(int byte);

	private:
		unsigned short portA0;
		unsigned short portB0;
		unsigned short portC0;
		unsigned short confReg0;
		unsigned short portA1;
		unsigned short portB1;
		unsigned short portC1;
		unsigned short confReg1;
		unsigned short directionReg;
		unsigned short gateReg;
};

#endif
