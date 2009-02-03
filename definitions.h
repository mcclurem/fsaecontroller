
//this is a file of #define's to allow us to set
//io port values



#ifndef HWMONCHIP
#define HWMONCHIP


	#define PWMCONFIG1	0x5C
	#define PWMCONFIG2	0x5D
	#define PWMCONFIG3	0x5E

	#define LOWTEMP1	0x67
	#define LOWTEMP2	0x68
	#define LOWTEMP3	0x69

	#define RANGEFREQ1	0x5F
	#define RANGEFREQ2	0x60
	#define RANGEFREQ3	0x61

	#define DUTY_CYCLE1	0x30
	#define DUTY_CYCLE2	0x31
	#define DUTY_CYCLE3	0x32

	#define TACH_1_LSB	0x28
	#define TACH_1_MSB	0x29
	#define TACH_2_LSB	0x2A
	#define TACH_2_MSB	0x2B
	#define TACH_3_LSB	0x2C
	#define TACH_3_MSB	0x2D

#endif

#ifndef WORDTYPE
#define WORDTYPE

inline unsigned short bitof(unsigned char _bit_number, unsigned short _data)
{
	return ((_data >> _bit_number) & 0x01);
};

union word{
		struct {
			//NOTE THE ORDER!!!!!!, THIS IS BECAUSE OF LITTLE ENDIAN ARCHITECHTURE
				unsigned char lsb, msb;
		};
		unsigned short value;
};

#endif

//**************Analog Ins(VCMDAS)***************
//0 = Gas pedal


//**************Digital Outs**************
//Bit			0 |  1  |  2  |  3  |  4  |     5       |   6   |   7
//Function		N | 1st | 2nd | 3rd | 4th | Shift light | Stat1 | Stat2
//
//Bit			  0    |    1     |     2      |
//Function		Clutch | Up-Shift | Down-Shift |
//
//**************Digital Ins***************
//
//Bit			     0     |     1       |   2    |    3     |    4   |    5     |    6       |
//Function		Gas-Enable | Elec-Enable | E-Stop | Ignition | Clutch | Shift-Up | Shift-Down |
