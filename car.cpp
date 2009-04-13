
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
		das = VCMDAS();
		board = PythonIO();
		
		das->setDigitalDirection(2, false);
		setPWMDuty(ThrottleCalc(0.));
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
	throttleOutPrevious = throttleOut;
	throttleOut = ThrottleCalc(pedalPosition);

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
