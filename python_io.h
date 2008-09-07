
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
	private:
		void initDigitalIO(unsigned int direction);
		void initPWM();
		/**
		 * Function to get one byte of digital input
		 * @param io_byte value from 0-3, corresponding to the byte we are interested in
		 */
		unsigned char getDigital(char io_byte);
		/**
		 * Function to get one byte of digital input
		 * @param io_byte value from 0-3, corresponding to the byte we are interested in
		 * @param _data Value to write to the byte
		 */
		void setDigital(char io_byte, unsigned char _data);
		/**
		 * Function to get analog input
		 * @param io_byte value from 0-3, corresponding to the byte we are interested in
		 */
		unsigned short getAnalog(char io_pin);
		void setPWMDuty(char _duty);
		void waitOnTransaction();

		unsigned short rSPIControl;//R/W
		unsigned short rSPIStatus;//R/W
		unsigned short rSPIData0;
		unsigned short rSPIData1;
		unsigned short rSPIData2;
		unsigned short rSPIData3;
};

#endif

