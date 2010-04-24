
#include "pcm.h"

PCM::PCM(unsigned short base_address)
{
	portA0 = base_address + 0;
	portB0 = base_address + 1;
	portC0 = base_address + 2;
	confReg0 = base_address + 3;
	portA1 = base_address + 4;
	portB1 = base_address + 5;
	portC1 = base_address + 6;
	confReg1 = base_address + 7;
	directionReg = base_address + 8;
	gateReg = base_address + 9;

	//Set all our ports up as inputs
	outb(0x9B, confReg0);
	outb(0x9B, confReg1);

}

unsigned char PCM::getDigital(int byte)
{
	unsigned short address;
	switch(byte)
	{
		case 0: address = portA0; break;
		case 1: address = portB0; break;
		case 2: address = portC0; break;
		case 3: address = portA1; break;
		case 4: address = portB1; break;
		case 5: address = portC1; break;
		default: address = portA0;
	};
	return inb(address);
}

