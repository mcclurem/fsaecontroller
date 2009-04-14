
#include "car.h"


inline unsigned char ThrottleCalc(float percentage)
{
	const unsigned char max_duty = 40; // using this so that we can adjust our endpoints of servo travel
	const unsigned char min_duty = 22;
	unsigned char retval;
	retval = (unsigned char)(((float)(max_duty - min_duty) * percentage) + (float)min_duty);
	if(retval < min_duty)
			retval = min_duty;
	if(retval > max_duty)
			retval = max_duty;
	return retval;
};

Car::Car()
{
		das = new VCMDAS();
		board = new PythonIO();
		
		das->setDigitalDirection(2, false);
		board->setPWMDuty(ThrottleCalc(0.));
		board->initDigitalIO(0xFF0000FF);
}



void Car::run()
{
	do
	{
	}while(true);
}



void Car::gasLoop()
{
	inputQuery();

//Before setting new throttle output, store previous value for hysterisis
	throttleOutPrevious = throttleOut;
	throttleOut = ThrottleCalc(pedalPosition);

//Fans = with some hysterisis
	fanHandler();

	writeOutputs();
}


void Car::electricLoop()
{
}



void Car::hybridLoop()
{
}



void Car::inputQuery()
{
	digIn1 = board->getDigital(0);
	digIn2 = board->getDigital(3);
	
	//Now we go ahead and pull all the VCMDas analog inputs
	pedalPosition = ((float)das->getRawAnalog(0))/32767.;
	brakePosition = ((float)das->getRawAnalog(1))/32767.;

	//These all need calibrating
	waterTemp = ((float)das->getRawAnalog(2))/32767.;
	throttlePosition = ((float)das->getRawAnalog(3))/32767.;
	bankVoltage = ((float)das->getRawAnalog(4))/32767.;
	shiftPressure = ((float)das->getRawAnalog(5))/32767.;

	cap0Temp = ((float)das->getRawAnalog(6))/32767.;
	cap1Temp = ((float)das->getRawAnalog(7))/32767.;
	cap2Temp = ((float)das->getRawAnalog(8))/32767.;
	cap3Temp = ((float)das->getRawAnalog(9))/32767.;
	cap4Temp = ((float)das->getRawAnalog(10))/32767.;
	cap5Temp = ((float)das->getRawAnalog(11))/32767.;
	
	frontLeftRPM = ((float)das->getRawAnalog(12))/32767.;
	frontRightRPM = ((float)das->getRawAnalog(13))/32767.;
	rearLeftRPM = ((float)das->getRawAnalog(14))/32767.;
	rearRightRPM = ((float)das->getRawAnalog(15))/32767.;

	engineRPM = board->getFreq(0);
}



void Car::writeOutputs()
{
	//We throw in a little hysteresis here in order to prevent
	//spazzing of the servo
	if(throttleOutPrevious != throttleOut)
		board->setPWMDuty(throttleOut);

	//Write our two python digital outs
	board->setDigital(1, digOut1);
	board->setDigital(2, digOut2);

	//Get our cap status
	word dasIn;
	dasIn.value = das->getDigital();
	capWarn = dasIn.lsb;
	capStop = dasIn.msb;

	//Write out to the electric
	unsigned short throt = (unsigned short)(electricThrottlePercentage * 4095.);
	unsigned short regen = (unsigned short)(electricRegenPercentage * 4095.);
	das->setAnalog(0, throt);
	das->setAnalog(1, regen);
}


//These threshold values need tweaked
inline void Car::fanHandler()
{
	if(bitof(4,digOut2))
	{
		if(waterTemp < 150.)
				bitunset(4, &digOut2);
	}else
	{
		if(waterTemp > 170.)
				bitset(4, &digOut2);
	}

}


inline void Car::shiftHandler()
{

}
