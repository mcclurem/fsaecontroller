//This is going to be a set of constants (and possibly functions) for working with the VCM-DAS2

#include <sys/io.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>



#ifndef VCMDAS_H
#define VCMDAS_H

class VCMDAS
{
	public:
		VCMDAS(unsigned short base_address, unsigned short _direction);
		
		double getAnalog(unsigned char channel);
		void setAnalog(unsigned char channel, unsigned short data);
		
		void initDigital();
		unsigned short getDigital();
		void setDigital(unsigned short data);
		void setDigitalDirection(int byte, bool input);
		
		void enableEEPROMWrite();
		unsigned int readEEPROM(unsigned int address);
		bool writeEEPROM(unsigned int address, unsigned int data);
		void writeDPot(unsigned int channel, unsigned int value);
		void loadConfig(unsigned int base_address);
	private:
		unsigned char dataControl;//Contains the data to be written to rControl (since we can't read it)
		unsigned short rControl;//W
		unsigned short rADStatus;//R
		unsigned short rADSelect;//W only
		unsigned short rADConvert;//W only
		unsigned short rADPeriodicTrig;//W only
		unsigned short rADLowByte;//R only
		unsigned short rADHighByte;//R only
		unsigned short rDigLowByte;//R/W
		unsigned short rDigHighByte;//R/W
		unsigned short rSPISelect;//W only
		unsigned short rSPIData;//R/W

};
#endif

