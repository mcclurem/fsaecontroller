
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

}

void PCM::getDigital(int byte)
{
}
