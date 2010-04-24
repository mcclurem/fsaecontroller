
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

#ifndef IOCHIP_REGS
#define IOCHIP_REGS
	#define GPPUA		0x0C
	#define GPPUB		0x0D
	#define IOCON		0x0A
	#define CHIP0		0x26
	#define CHIP1		0x27
	#define WRITECMD	0x40
	#define IODIRA		0x00
	#define IODIRB		0x01
	#define BUSCONFIG	0x30
#endif



#ifndef WORDTYPE
#define WORDTYPE

inline bool bitof(unsigned char _bit_number, unsigned short _data)
{
	return ((_data >> _bit_number) & 0x01);
}

inline void bitset(unsigned char bit, unsigned char* byte)
{
		if( bit > 7 )
				return;
		*byte |= (0x01 << bit);
}

inline void bitunset(unsigned char bit, unsigned char* byte)
{
		if( bit > 7 )
				return;
		*byte &= (~(0x01 << bit));
}

union word{
		struct {
			//NOTE THE ORDER!!!!!!, THIS IS BECAUSE OF LITTLE ENDIAN ARCHITECHTURE
				unsigned char lsb, msb;
		};
		unsigned short value;
};

#endif

//**************Analog Outs(VCMDAS)***************
//00 = Electric Throttle
//01 = Electric Regen
//
//**************Analog Ins(VCMDAS)***************
//00 = Gas pedal
//01 = Brake pedal
//02 = Water temp
//03 = TPS
//04 = Bank Voltage
//05 = Gas Pressure
//06 = Cap 0 temp
//07 = Cap 1 temp
//08 = Cap 2 temp
//09 = Cap 3 temp
//10 = Cap 4 temp
//11 = Cap 5 temp
//12 = Front Left RPM
//13 = Front Right RPM
//14 = Rear Left RPM
//15 = Rear Right RPM
//
//*************Digital Ins(VCMDAS)**************
//00-06 = Cap 0-6 Warn Signal
//07 = Free
//08-14 = Cap 0-6 Stop Signal
//15 = Free
//
//



//
//**************Onboard IOs***************
//
//IN			     0     |     1       |   2   |  3  |  4  |    5     |    6       |    7
//Function		Gas-Enable | Elec-Enable | Start | SW1 | SW2 | Shift-Up | Shift-Down | Clutch
//
//OUT			 8   |  9   |  10  |  11  |  12  |    13    |    14     |         15
//Function		LED1 | LED2 | LED3 | LED4 | LED5 | Throt-EN | Brake-EN  | Motor Controller-EN
//
//OUT			  16   |   17     |    18      |    19   |    20   |    21   |   22   |   23
///Function		Clutch | Shift-up | Shift-down | Bank-en | Fans-en | Fuel-en | ign-en | Starter
//
//IN			  24   |      25      |     26     |    27   |  28  |  29  |  30  |  31
//Function		E-stop | Oil Pressure | Boot-Linux | Neutral | Free | Free | Free | Free
