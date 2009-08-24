
#include "python_io.h"


PythonIO::PythonIO()
{
		rSPIControl = 0x1D8;
		rSPIStatus = 0x1D9;

		rSPIData0 = 0x1DA;
		rSPIData1 = 0x1DB;
		rSPIData2 = 0x1DC;
		rSPIData3 = 0x1DD;

		rHWMoniterIndex = 0xC70;
		rHWMoniterData = 0xC71;

		iopl(3);	//Allow us access to ALL ports. We will need root permission and it should
						//be noted that This is moderately dangerous, however we will be careful.
						//I promise.
		//Init_Onboard_Digital();
		initPWM();
		initDigitalIO(0xFFFFFFFF);//Set everything to inputs for now
}



unsigned char PythonIO::getDigital(char io_byte)
{
	
	//****NB****
	//THIS ENTIRE METHOD IS SPECIFIC TO THE REV6 BOARD
	//PORTA and PORTB are swapped!!!!
		unsigned char gpio_address, chip_address, tmp_var;
		if(bitof(0, io_byte))
			gpio_address = 0x12; //GPIO bank B = 0x13
		else
			gpio_address = 0x13; //GPIO bank A = 0x12

		if(bitof(1, io_byte))
			outb( 0x27, rSPIControl ); //Dig I/O chip 1 (see ref manual for bitwise breakdown)
		else
			outb( 0x26, rSPIControl ); //Dig I/O chip 0 (see ref manual for bitwise breakdown)

		outb( 0x30, rSPIStatus ); //irq3, 8.3MHz, int disabled, shift Msb
		outb( 0x00, rSPIData1 );	//We're not acctually writing anything to the bank so its irrellevant
		outb( gpio_address, rSPIData2 );	//Choose the GPIO bank on the chip
		outb( 0x41, rSPIData3 );			//The R/W bit (bit 0) should be 1 == read
		waitOnTransaction();

		tmp_var = inb(rSPIData1);
		return tmp_var;
}

void PythonIO::setDigital(char io_byte, unsigned char _data)
{
	//****NB****
	//THIS ENTIRE METHOD IS SPECIFIC TO THE REV6 BOARD
		
		char gpio_address, chip_address;
		if(bitof(0, io_byte))
			gpio_address = 0x12; //GPIO bank B = 0x13
		else
			gpio_address = 0x13; //GPIO bank A = 0x12


		if(bitof(1, io_byte))
			outb( 0x27, rSPIControl ); //Dig I/O chip 1 (see ref manual for bitwise breakdown)
		else
			outb( 0x26, rSPIControl ); //Dig I/O chip 0 (see ref manual for bitwise breakdown)
		
		outb( 0x30, rSPIStatus ); //irq3, 8.3MHz, int disabled, shift Msb
		outb( _data, rSPIData1);			//Data to write to the bank
		outb( gpio_address, rSPIData2);		//Choose the GPIO bank on the chip
		outb( 0x40, rSPIData3);			//The R/W bit (bit 0) should be 0 == Write
		waitOnTransaction();
}





//Actually only a 12 bit number but we wrap it in an unsigned short
unsigned short PythonIO::getAnalog(char io_pin)
{
		word analog_value;
		io_pin = io_pin << 3;	//We bit shift it because it is supposed to write the address in pins 5-3
		
		outb( 0x35, rSPIControl ); //A/D chip select with 32bit frame for single sample,see ref manual
		outb( 0x30, rSPIStatus ); //irq3, 8.3MHz, int disabled, shift Msb
		outb( 0x00, rSPIData2 );	//Unread data so value doesn't matter
		outb(io_pin, rSPIData3);	//Initiate the transaction by writing the port value
		waitOnTransaction();
		
		analog_value.lsb = inb(rSPIData0);
		analog_value.msb = inb(rSPIData1);
		return analog_value.value;

		//Alt single instruction
		//return inw(rSPIData0);
}


//Function to initialize and configure the digital ports
void PythonIO::initDigitalIO(unsigned int direction)
{
	//All-right, theres a fair ammount of confusion as to what is what
	//according to the beginning of the manual:
	//SPIDATA1 = Data byte
	//SPIDATA2 = Address Byte
	//SPIDATA3 = command byte - Which I'm guessing is allways 0x40
	//Not sure what I believe, but we're going to use that assumption to clean up their code
	
	outb( 0x26, rSPIControl );	//Chip-select first on-board Digital I/O
	outb( 0x30, rSPIStatus );	//irq3, 8.3MHz, int disabled, shift Msb
	outb( 0xFF, rSPIData1 );
	outb( 0x0C, rSPIData2 ); //GPPUA
	outb( 0x40, rSPIData3 );	//The R/W bit(bit 0) should be 0 == write
	waitOnTransaction();
	
	outb( 0x26, rSPIControl );	//Chip-select first on-board Digital I/O
	outb( 0x30, rSPIStatus );	//irq3, 8.3MHz, int disabled, shift Msb
	outb( 0xFF, rSPIData1 );
	outb( 0x0D, rSPIData2 ); //GPPUB
	outb( 0x40, rSPIData3 );	//The R/W bit(bit 0) should be 0 == write
	waitOnTransaction();
	
	
	unsigned char byte0, byte1, byte2, byte3;
	byte0 = (direction & 0x000000FF);
	byte1 = (direction & 0x0000FF00) >> 8;
	byte2 = (direction & 0x00FF0000) >> 16;
	byte3 = (direction & 0xFF000000) >> 24;

	printf("DIR 0=%d, DIR 1=%d, DIR 2=%d, DIR 3=%d\n", byte0, byte1, byte2, byte3);
		outb( 0x26, rSPIControl );	//Chip-select first on-board Digital I/O
		outb( 0x30, rSPIStatus );	//irq3, 8.3MHz, int disabled, shift Msb
		outb( byte0, rSPIData1 );	//Set all 1's to set all to inputs
		outb( 0x00, rSPIData2 );	//IODIRA register = 0x00
		outb( 0x40, rSPIData3 );	//The R/W bit(bit 0) should be 0 == write
		waitOnTransaction();

		outb( 0x26, rSPIControl );	//Chip-select first on-board Digital I/O
		outb( 0x30, rSPIStatus );	//irq3, 8.3MHz, int disabled, shift Msb
		outb( byte1, rSPIData1 );	//Set all 1's to set all to inputs
		outb( 0x10, rSPIData2 );	//IODIRB register = 0x10
		outb( 0x40, rSPIData3 );	//The R/W bit(bit 0) should be 0 == write
		waitOnTransaction();
	//We've now initialized the first chip
		outb( 0x27, rSPIControl );	//Chip-select second on-board Digital I/O
		outb( 0x30, rSPIStatus );	//irq3, 8.3MHz, int disabled, shift Msb
		outb( byte2, rSPIData1 );	//Set all 1's to set all to inputs
		outb( 0x00, rSPIData2 );	//IODIRA register = 0x00
		outb( 0x40, rSPIData3 );	//The R/W bit(bit 0) should be 0 == write
		waitOnTransaction();

		outb( 0x27, rSPIControl );	//Chip-select second on-board Digital I/O
		outb( 0x30, rSPIStatus );	//irq3, 8.3MHz, int disabled, shift Msb
		outb( byte3, rSPIData1 );	//Set all 1's to set all to inputs
		outb( 0x01, rSPIData2 );	//IODIRB register = 0x01
		outb( 0x40, rSPIData3 );	//The R/W bit(bit 0) should be 0 == write
		waitOnTransaction();
	//We've now initialized the second chip to all inputs


}
/*
void PythonIO::setPullup(bool enable)
{
	outb( 0x26, rSPIControl );	//Chip-select first on-board Digital I/O
	outb( 0x30, rSPIStatus );	//irq3, 8.3MHz, int disabled, shift Msb
	outb( 0x
}
*/
void PythonIO::initPWM()
{
		//ioperm only gives access to ports upto 0x3FF anything higher and we need to use iopl()
//		ioperm(BASEADDRESS, #OFPORTS, ADDORREMOVEACCESS(bool));

		char tmp_var;
		outb(0x40, rHWMoniterIndex);		// Waiting on more info as to what the actual pinout of this port
										//is, but it is essentially a register selector for the HWMon chip
										//(ie I have no friggin clue what 0x40 does, I can only assume
										//that it sets me to a config register on the chip because the ASM tells me to do it)
		tmp_var = inb(rHWMoniterData);		//We need to see the old value to modify it
		tmp_var = tmp_var & 0xFE;		// doing a bitmask to disable the first bit (0xFE==11111110)
		outb(tmp_var, rHWMoniterData); 		// Start bit is now off
// we've now disabled the chip

		outb(PWMCONFIG1, rHWMoniterIndex);	//Now we need to put the PWM into manual mode
		tmp_var = inb(rHWMoniterData);
		tmp_var = tmp_var | 0xE0;		//apparently the 3MSBs are in control of manual mode
										//however, there is some sloppy bitwise manipulation done in the ASM
										//so this should double-checked
		outb(tmp_var, rHWMoniterData);
//We've now put PWM1 into manual mode - need to do others if we want them
		
//We do in fact have to do the following to all of the PWM's even though we are only using one
		outb(LOWTEMP1, rHWMoniterIndex);
		outb(0x81, rHWMoniterData);			//We have to set it to a value other than the default (0x80) and they
										//suggested 0x81 in the example
		outb(LOWTEMP2, rHWMoniterIndex);
		outb(0x81, rHWMoniterData);
		outb(LOWTEMP3, rHWMoniterIndex);
		outb(0x81, rHWMoniterData);
//done setting low temp limit registers

//Now we set the PWM duty-cycle
		outb(DUTY_CYCLE1, rHWMoniterIndex);	//Chose the first pwm duty-cycle register
//***********************************************************
		outb(0x22, rHWMoniterData);			// we need to fix this value, it affects the position that the PWM is 
										//initialized at (ie the initial servo position)
//***********************************************************
//Done setting duty-cycle

//Set the PWM frequency
		outb(RANGEFREQ1, rHWMoniterIndex);
		tmp_var = inb(rHWMoniterData);	//we need to read in the old value because we need to rewrite the same upper
									//nibble, only the lower nibble affects frequency
		tmp_var = tmp_var & 0xF0;	// clear the lower nibble
		tmp_var = tmp_var | 0x07;	//87.7Hz, we may want to decide on a diff value depending on the servo we chose
		outb(tmp_var, rHWMoniterData);
//Done setting the frequency

//Now we have to re-enable the chip
		outb(0x40, rHWMoniterIndex);
		tmp_var = inb(rHWMoniterData);

		tmp_var = tmp_var | 0x01;	//Re-enable the start-bit
		outb(tmp_var, rHWMoniterData);
//Done re-enabling


}

float PythonIO::getFreq(int _chip)
{
		return 1./((float)getTach(_chip) * 0.0000111);
}

int PythonIO::getTach(int _chip)
{
	//LSB = 0x28, MSB = 0x29 ; + chip*2
		word foo;
		outb((0x28 + _chip * 2), rHWMoniterIndex); 
		foo.lsb = inb(rHWMoniterData);
		outb((0x29 + _chip * 2), rHWMoniterIndex);
		foo.msb = inb(rHWMoniterData);

		return foo.value;
}

void PythonIO::setPWMDuty(char _duty)
{
	outb(DUTY_CYCLE1, rHWMoniterIndex);	//choose the first pwm duty-cycle register
	outb(_duty, rHWMoniterData);
}


//Pause function for SPI transactions
void PythonIO::waitOnTransaction()
{
		char tmp_var;
		do
		{
				tmp_var = inb(rSPIStatus);	//Read in and we need to check if the done bit is set
				tmp_var = tmp_var & 0x01;	//Isolate the done bit
		}while(tmp_var == 0x01);
}

