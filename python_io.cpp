
#include "python_io.h"


PythonIO::PythonIO()
{
		rSPIControl = 0x1D8;
		rSPIStatus = 0x1D9;

		rSPIData0 = 0x1DA;
		rSPIData1 = 0x1DB;
		rSPIData2 = 0x1DC;
		rSPIData3 = 0x1DD;

		iopl(3);	//Allow us access to ALL ports. We will need root permission and it should
						//be noted that This is moderately dangerous, however we will be careful.
						//I promise.
		//Init_Onboard_Digital();
		initPWM();
}



unsigned char PythonIO::getDigital(char io_byte)
{
	
	//****NB****
	//THIS ENTIRE METHOD IS SPECIFIC TO THE REV6 BOARD
		
		unsigned char gpio_address, chip_address, tmp_var;
		if(bitof(0, io_byte))
			gpio_address = 0x13; //GPIO bank B = 0x13
		else
			gpio_address = 0x12; //GPIO bank A = 0x12

		if(bitof(1, io_byte))
			outb( 0x27, rSPIControl ); //Dig I/O chip 1 (see ref manual for bitwise breakdown)
		else
			outb( 0x26, rSPIControl ); //Dig I/O chip 0 (see ref manual for bitwise breakdown)

		outb( 0x30, rSPIStatus ); //irq3, 8.3MHz, int disabled, shift Msb
		outb( 0x00, rSPIData1 );	//We're not acctually writing anything to the bank so its irrellevant
		outb( gpio_address, rSPIData2 );	//Choose the GPIO bank on the chip
		outb( 0x41, rSPIData3 );			//The R/W bit (bit 0) should be 1 == read
		waitOnTransaction();

		tmp_var = inb(SPIDATA1);
		return tmp_var;
}

void PythonIO::setDigital(char io_byte, unsigned char _data)
{
	//****NB****
	//THIS ENTIRE METHOD IS SPECIFIC TO THE REV6 BOARD
		
		char gpio_address, chip_address;
		if(bitof(0, io_byte))
			gpio_address = 0x13; //GPIO bank B = 0x13
		else
			gpio_address = 0x12; //GPIO bank A = 0x12


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
void PythonIO::initDigitalIO(bool chip, unsigned short direction)
{
	//All-right, theres a fair ammount of confusion as to what is what
	//according to the beginning of the manual:
	//SPIDATA1 = Data byte
	//SPIDATA2 = Address Byte
	//SPIDATA3 = command byte - Which I'm guessing is allways 0x40
	//Not sure what I believe, but we're going to use that assumption to clean up their code
	
	unsigned char byte1, byte2, byte3, byte4;
		outb( 0x26, rSPIControl );	//Chip-select first on-board Digital I/O
		outb( 0x30, rSPIStatus );	//irq3, 8.3MHz, int disabled, shift Msb
		outb( 0xFF, rSPIData1 );	//Set all 1's to set all to inputs
		outb( 0x00, rSPIData2 );	//IODIRA register = 0x00
		outb( 0x40, rSPIData3 );	//The R/W bit(bit 0) should be 0 == write
		waitOnTransaction();

		outb( 0x26, rSPIControl );	//Chip-select first on-board Digital I/O
		outb( 0x30, rSPIStatus );	//irq3, 8.3MHz, int disabled, shift Msb
		outb( 0xFF, rSPIData1 );	//Set all 1's to set all to inputs
		outb( 0x01, rSPIData2 );	//IODIRB register = 0x01
		outb( 0x40, rSPIData3 );	//The R/W bit(bit 0) should be 0 == write
		waitOnTransaction();
	//We've now initialized the first chip to all inputs
		outb( 0x27, rSPIControl );	//Chip-select first on-board Digital I/O
		outb( 0x30, rSPIStatus );	//irq3, 8.3MHz, int disabled, shift Msb
		outb( 0xFF, rSPIData1 );	//Set all 1's to set all to inputs
		outb( 0x00, rSPIData2 );	//IODIRA register = 0x00
		outb( 0x40, rSPIData3 );	//The R/W bit(bit 0) should be 0 == write
		waitOnTransaction();

		outb( 0x27, rSPIControl );	//Chip-select first on-board Digital I/O
		outb( 0x30, rSPIStatus );	//irq3, 8.3MHz, int disabled, shift Msb
		outb( 0xFF, rSPIData1 );	//Set all 1's to set all to inputs
		outb( 0x01, rSPIData2 );	//IODIRB register = 0x01
		outb( 0x40, rSPIData3 );	//The R/W bit(bit 0) should be 0 == write
		waitOnTransaction();
	//We've now initialized the second chip to all inputs


}



void PythonIO::initPWM()
{
		//ioperm only gives access to ports upto 0x3FF anything higher and we need to use iopl()
//		ioperm(BASEADDRESS, #OFPORTS, ADDORREMOVEACCESS(bool));

		char tmp_var;
		outb(0x40, HWMONINDEX);		// Waiting on more info as to what the actual pinout of this port
										//is, but it is essentially a register selector for the HWMon chip
										//(ie I have no friggin clue what 0x40 does, I can only assume
										//that it sets me to a config register on the chip because the ASM tells me to do it)
		tmp_var = inb(HWMONDATA);		//We need to see the old value to modify it
		tmp_var = tmp_var & 0xFE;		// doing a bitmask to disable the first bit (0xFE==11111110)
		outb(tmp_var, HWMONDATA); 		// Start bit is now off
// we've now disabled the chip

		outb(PWMCONFIG1, HWMONINDEX);	//Now we need to put the PWM into manual mode
		tmp_var = inb(HWMONDATA);
		tmp_var = tmp_var | 0xE0;		//apparently the 3MSBs are in control of manual mode
										//however, there is some sloppy bitwise manipulation done in the ASM
										//so this should double-checked
		outb(tmp_var, HWMONDATA);
//We've now put PWM1 into manual mode - need to do others if we want them
		
//We do in fact have to do the following to all of the PWM's even though we are only using one
		outb(LOWTEMP1, HWMONINDEX);
		outb(0x81, HWMONDATA);			//We have to set it to a value other than the default (0x80) and they
										//suggested 0x81 in the example
		outb(LOWTEMP2, HWMONINDEX);
		outb(0x81, HWMONDATA);
		outb(LOWTEMP3, HWMONINDEX);
		outb(0x81, HWMONDATA);
//done setting low temp limit registers

//Now we set the PWM duty-cycle
		outb(DUTY_CYCLE1, HWMONINDEX);	//Chose the first pwm duty-cycle register
//***********************************************************
		outb(0x80, HWMONDATA);			// we need to fix this value, it affects the position that the PWM is 
										//initialized at (ie the initial servo position)
//***********************************************************
//Done setting duty-cycle

//Set the PWM frequency
		outb(RANGEFREQ1, HWMONINDEX);
		tmp_var = inb(HWMONDATA);	//we need to read in the old value because we need to rewrite the same upper
									//nibble, only the lower nibble affects frequency
		tmp_var = tmp_var & 0xF0;	// clear the lower nibble
		tmp_var = tmp_var | 0x06;	//58.6Hz, we may want to decide on a diff value depending on the servo we chose
//Done setting the frequency

//Now we have to re-enable the chip
		outb(0x40, HWMONINDEX);
		tmp_var = inb(HWMONDATA);

		tmp_var = tmp_var | 0x01;	//Re-enable the start-bit
		outb(tmp_var, HWMONDATA);
//Done re-enabling


}


void PythonIO::setPWMDuty(char _duty)
{
	outb(DUTY_CYCLE1, HWMONINDEX);	//choose the first pwm duty-cycle register
	outb(_duty, HWMONDATA);
}


//Pause function for SPI transactions
void PythonIO::waitOnTransaction()
{
		char tmp_var;
		do
		{
				tmp_var = inb(rSPIStatus);	//Read in and we need to check if the done bit is set
				tmp_var = tmp_var & 0x01;	//Isolate the done bit
		}while(tmp_var != 0x01);
}

