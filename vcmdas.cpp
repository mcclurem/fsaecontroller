

#include "vcmdas.h"
#include "definitions.h"

VCMDAS::VCMDAS(unsigned short base_address)
{
		iopl(3);	//Allow us access to ALL ports. We will need root permission and it should
	//need to initialize all the register address variables
		rControl = base_address;
		rADStatus = base_address;
		rADSelect = base_address + 1;
		rADConvert = base_address + 2;
		rADPeriodicTrig = base_address + 3;
		rADLowByte = base_address + 4;
		rADHighByte = base_address + 5;
		rDigLowByte = base_address + 6;
		rDigHighByte = base_address + 7;
		rSPISelect = base_address + 8;
		rSPIData = base_address + 9;
	//Done initializing register variables
		
		setDigitalDirection(2, 1); //Sets both bytes to inputs
		loadConfig(0);


}


void VCMDAS::initDigital()
{
	//Control register bit mapping:
	//7=High Direction
	//6=Low Direction
	//5=DMA enable
	//4=Auto incr enable
	//3=Auto trigger : see refmanual this could be useful
	//2=periodictrigger enable
	//1=Shared interrupt enable
	//0=AD interrupt enable
	dataControl = 0xC0;
	outb_p(dataControl, rControl);
}

unsigned short VCMDAS::getDigital()
{
		int retval;
		retval = inw(rDigLowByte);
		return retval;
}

void VCMDAS::setDigital(unsigned short data)
{
	outw_p(data, rDigLowByte);
}

//Byte = 0, 1, 2 == both
void VCMDAS::setDigitalDirection(int byte, bool input)
{
	byte++; //Increment so we can use the first two digits in bitwise set vs unset tests
	if(input)
	{
		if(bitof(0, byte))
			dataControl = dataControl & 0xBF;	//Unset bit 6 because 0==input
		if(bitof(1, byte))
			dataControl = dataControl & 0x7F;	//Unset bit 7 because 0==input
	}
	else
	{
		if(bitof(0, byte))
			dataControl = dataControl | 0x40;	//Set bit 6 because 1==output
		if(bitof(1, byte))
			dataControl = dataControl | 0x80;	//Set bit 7 because 1==output
	}
	outb_p(dataControl, rControl);
}

short VCMDAS::getRawAnalog(unsigned char channel) //THIS IS A Bi-Polar INPUT (hence the signed return val)
{
	//*******************ALTERNATIVE, SINGLE INSTRUCTION*******
		
	//If this bit is set, it means we have an old conversion getting moldy just sitting in the register
		if( bitof(6, inb(rADStatus)) )
			short foo = inw(rADLowByte);

	//By using a word wide write to the select register, we force a settling time
	//delay but its only possible to avoid that if we KNOW that the previous
	//conversion was on the same channel
		word out_word;
	//on the word wide write, we want the channel byte to go into the first
	//register so we use the lsb because of endianess
		out_word.lsb = channel;
		out_word.msb = 0x01;	
		outw(out_word.value, rADSelect);

//************WARNING WARNING WARNING******************
//THIS IS CAPABLE OF CREATING AN INFINITE LOOP
//Added timedout variable to get rid of infinite loops
		int timedout = 1000;
		do{
			timedout--;
			unsigned char tmpval = inb(rADStatus);
			if(bitof(7,tmpval))
			{
				continue;
			}else
			{
				if(bitof(6,tmpval))
				{
					break;
				}
			}
		}while(true && timedout);
		if(timedout)
		{
			return inw(rADLowByte);
//			return inw(rADLowByte) * 0.0001525878907; // assumes +-5V in
//			return inw(rADLowByte) * 0.0003051757813; // if +-10V input
		}
		else
			return 0;
}
double VCMDAS::getAnalog(unsigned char channel)
{
		return getRawAnalog(channel) * 0.0001525878907; // assumes +-5V in
//		return getRawAnalog(channel) * 0.0003051757813; // if +-10V input
}

void VCMDAS::setAnalog(unsigned char channel, unsigned short data)
{

	outb(0x01,rSPISelect);	//Route all data written to SPIWDAT register to DAC chip (also clears DACLOAD bit)
	//Not sure why, but we must write out one bit at a time (god this is some drunk shit)
	outb(0x01, rSPIData);	//1=pay attention to a/b bits, 0=load both chips
	if(channel == 0)
	{
		outb(0x00, rSPIData);
		outb(0x01, rSPIData);
	}else
	{
		outb(0x01, rSPIData);
		outb(0x00, rSPIData);
	}
	outb(0x00, rSPIData);	//unused, write as 0
	//following 12 writes are the apropriate bits for data
	outb(bitof(11,data), rSPIData);
	outb(bitof(10,data), rSPIData);
	outb(bitof(9, data), rSPIData);
	outb(bitof(8, data), rSPIData);
	outb(bitof(7, data), rSPIData);
	outb(bitof(6, data), rSPIData);
	outb(bitof(5, data), rSPIData);
	outb(bitof(4, data), rSPIData);
	outb(bitof(3, data), rSPIData);
	outb(bitof(2, data), rSPIData);
	outb(bitof(1, data), rSPIData);
	outb(bitof(0, data), rSPIData);
	outb(0x08, rSPISelect);	//Set DACLOAD bit to load correct A/D
	outb(0x01, rSPISelect);	//Unset bit

}

//Stolen heavily from their demo code
unsigned int VCMDAS::readEEPROM(unsigned int address)
{
		outb_p(0x04, rSPISelect); //Choose the eeprom on SPI bus

		/*-----------------------------------------------
		 * Shift vaule:  9 bits
			SB OP OP A5 A4 A3 A2 A1 A0
			-- -- -- -- -- -- -- -- --
			 1  1  0  X  X  X  X  X  X
		 *-----------------------------------------------*/
		
		//begin bit banging
		outb_p(0x01, rSPIData);
		outb_p(0x01, rSPIData);
		outb_p(0x00, rSPIData);
		//done with opcode, now write the address
		 for(int i=0; i < 6; i++)
		 {
				 outb_p(((address&0x0020)>>5), rSPIData );
				 address = (address << 1);
		 }

		 unsigned int data = 0;
		 for(int i=0; i < 16; i++)
		 {
				data = (data << 1);			// Make space for the next data bit
				outb_p(0x00, rSPIData);		// Dummy output to clock data
				data = (data | (0x01 & inb(rSPIData)) );
		 }
		 outb_p(0x00, rSPISelect);
		 return data;

}

//Stolen heavily from their demo code
bool VCMDAS::writeEEPROM(unsigned int address, unsigned int data)
{
		outb_p(0x04, rSPISelect); //Choose the eeprom on SPI bus
		/*---------------------------------------------------------------------------------
		 * Shift value:  25 bits
			SB OP OP A5 A4 A3 A2 A1 A0 D15 D14 D13 D12 D11 D10 D9 D8 D7 D6 D5 D4 D3 D2 D1 D0
			 1  0  1
		 *---------------------------------------------------------------------------------*/
		outb_p(0x01, rSPIData);
		outb_p(0x00, rSPIData);
		outb_p(0x01, rSPIData);
		/* Output A5-A0 */
		for (int i=0; i < 6; i++)
		{
			outb_p(( (address&0x0020) >>5), rSPIData);
			address = (address << 1);
		}

		/* Output D15-D0 */
		for (int i=0; i < 16; i++)
		{
			outb_p( ((data&0x8000)>>15), rSPIData );
			data = (data << 1);
		}
		/* Enter busy loop with timeout escape */
		bool done = inb(rSPIData) & 0x01;
		for( int timedout = 30000; !done && timedout; timedout-- )
		{
			done = inb(rSPIData) & 0x01;
		}
		
		/* Drop the EEPROM chip select */
		outb_p(0x00, rSPISelect);
		usleep(10);
		if( !done )
			return false;
		return true;
}

void VCMDAS::enableEEPROMWrite()
{
		outb(0x04, rSPISelect); //Choose the eeprom on SPI bus
		outb(0x01, rSPIData);//Start bit
		outb(0x00, rSPIData);//Opcode = 00
		outb(0x00, rSPIData);
		for(int i=0; i < 6; i++)
			outb(0x01, rSPIData);//Fill out the byte with ones
		outb(0x00, rSPISelect); //Drop EEPROM chip select
}

void VCMDAS::writeDPot(unsigned int channel, unsigned int value)
{
		outb(0x02, rSPISelect); //Choose the dpot chip on the SPI bus
		outb((channel>>1), rSPIData); //Output address 1
		outb((channel&0x01), rSPIData); //Output address 0
		for(int i=0; i < 8; i++)
		{
				outb(((value&0x0080)>>7), rSPIData);
				value = (value << 1);
		}
		for(int i=0; i < 20000; i++)
				outb(0x00, rSPISelect);
}

void VCMDAS::loadConfig(unsigned int base_address)
{
		writeDPot(0, readEEPROM(base_address + 0));
		writeDPot(1, readEEPROM(base_address + 1));
		writeDPot(2, readEEPROM(base_address + 2));
		writeDPot(3, readEEPROM(base_address + 3));
}

