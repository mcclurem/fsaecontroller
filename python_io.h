
#include "definitions.h"
#include <sys/io.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef PYTHON_IO_H
#define PYTHON_IO_H

class PythonIO
{
	public:
		PythonIO();
		void setPWMDuty(char _duty);
		int getTach(int _chip);
		float getFreq(int _chip);
		/**
		 * Function to set one byte of digital output
		 * @param io_byte value from 0-3, corresponding to the byte we are interested in
		 * @param _data Value to write to the byte
		 */
		void setDigital(char io_byte, unsigned char _data);
		void initDigitalIO(unsigned int direction);
		void initPWM();
		/**
		 * Function to get one byte of digital input
		 * @param io_byte value from 0-3, corresponding to the byte we are interested in
		 */
		unsigned char getDigital(char io_byte);
		/**
		 * Function to get analog input
		 * @param io_byte value from 0-3, corresponding to the byte we are interested in
		 */
		unsigned short getAnalog(char io_pin);
	private:
		void waitOnTransaction();

		unsigned short rSPIControl;//R/W
		unsigned short rSPIStatus;//R/W
		unsigned short rSPIData0;
		unsigned short rSPIData1;
		unsigned short rSPIData2;
		unsigned short rSPIData3;

		unsigned short rHWMoniterIndex;
		unsigned short rHWMoniterData;
};

#endif

