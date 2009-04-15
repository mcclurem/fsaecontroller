
#include "car.h"

#define GAS_SWITCH (bitof(0, digIn1))
#define ELECTRIC_SWITCH (bitof(1, digIn1))
#define ESTOP (bitof(0, digIn2))

#define BANK_OFF bitunset(3, &digOut2)
#define BANK_ON bitset(3, &digOut2)

#define FUEL_OFF bitunset(5, &digOut2)
#define FUEL_ON bitset(5, &digOut2)

#define MC_OFF bitunset(7, &digOut1)
#define MC_ON bitset(7, &digOut1)

#define IGN_OFF bitunset(6, &digOut2)
#define IGN_ON bitset(6, &digOut2)

#define FAN_OFF bitunset(4, &digOut2)
#define FAN_ON bitset(4, &digOut2)



Car::Car()
{
		das = new VCMDAS();
		board = new PythonIO();
		
		das->setDigitalDirection(2, false);
		throttleCalc(0.);
		board->setPWMDuty(throttleOut);
		board->initDigitalIO(0xFF0000FF);
}



void Car::run()
{
	inputQuery();
	do
	{
		inputQuery();//Query the ins to simplify each loop
		
		//Hybrid
		if(GAS_SWITCH && ELECTRIC_SWITCH && !ESTOP)
		{
			//need to set some state variables on first entry:
			if( mode != HYBRID )
			{
				MC_ON;
				BANK_ON;
				FUEL_ON;
				IGN_ON;
			//Set the Mode state variable
				mode = HYBRID;
			}
				hybridLoop();
		}


		//Gas
		if(GAS_SWITCH && !ELECTRIC_SWITCH && !ESTOP)
		{
			//need to set some state variables on first entry:
			if( mode != GAS )
			{
				MC_OFF;
				BANK_OFF;
				FUEL_ON;
				IGN_ON;
			//Set the Mode state variable
				mode = GAS;
			}
			gasLoop();

		}
		

		//Electric
		if(!GAS_SWITCH && ELECTRIC_SWITCH && !ESTOP)
		{
			//need to set some state variables on first entry:
			if( mode != ELECTRIC )
			{
				MC_ON;
				BANK_ON;
				FUEL_OFF;
				IGN_OFF;
				FAN_OFF;
				throttleCalc(0.); //Close the throttle
			//Set the Mode state variable
				mode = ELECTRIC;
			}
			electricLoop();
		}


		//FAULT
		if(!bitof(0,digIn1) && !bitof(1, digIn1))
		{
			//need to set some state variables on first entry:
			if( mode != FAULT )
			{
				FAN_OFF;
				FUEL_OFF;
				BANK_OFF;
				MC_OFF;
				IGN_OFF;
				throttleCalc(0.); //Close the throttle
			//Set the Mode state variable
				mode = FAULT;
			}
		}


		writeOutputs();
	}while(true);
}



void Car::gasLoop()
{

	throttleCalc(pedalPosition);

//Fans with some hysterisis
	fanHandler();
//Handle shifting
	shiftHandler();

//If tach is less than 500RPM then engine is not running
//This block is a very simple starting routine, setting this particular
//RPM value accurately is going to be important
	if( engineRPM < 500. )
	{
		ensureNeutral();
		//very simpler starting routine
		if( bitof(2, digIn1) ) //Is the starter being pressed?
		{
			bitset(7, &digOut2); //Starter relay on
		}else
		{
			bitset(4, &digOut1); // throw up a fault LED
			return;
		}
	}else
	{
		bitunset(7, &digOut2); //Starter relay off
	}


}


void Car::electricLoop()
{
	shiftHandler();
	if(brakePosition < 0.05)
	{
		electricThrottlePercentage = pedalPosition;
		electricRegenPercentage = 0.;
		bitset(5, &digOut1); //ThrottleEnable
		bitunset(6, &digOut1); //Regen Disable
	}else
	{
		electricThrottlePercentage = 0.;
		electricRegenPercentage = brakePosition;
		bitunset(5, &digOut1); //ThrottleDisable
		bitset(6, &digOut1); //Regen Enable
	}
}



void Car::hybridLoop()
{
	float currentEngineTorque = gasMap->lookup(engineRPM, throttlePosition);
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
//Because of the importance of avoiding false triggering,
//we query the inputs again (note that this is done with
//short-circuiting so we aren't adding to runtime except
//when necessary
	//upshift routine
		if(bitof(5, digIn1) && bitof(5, board->getDigital(0)))
		{
				
		//Clutch
			bitset(0, &digOut2); //Clutch on
			board->setDigital(2, digOut2); //Trigger it
			usleep(10000); //10Msec sleep to allow clutch to pop
		//Shift
			bitset(1, &digOut2); //We throw the up shift
			board->setDigital(2, digOut2); //Trigger it
			usleep(10000); // 10Msec sleep to allow shift to occur
		//Release
			bitunset(1, &digOut2); //Now we release the shift
			board->setDigital(2, digOut2); //Trigger it
			usleep(10000); // 10Msec sleep to allow shifter to rebound

			bitunset(0, &digOut2); //Clutch release
		}	


	//downshift routine
		if(bitof(6, digIn1) && bitof(6, board->getDigital(0)))
		{
		//Clutch
			bitset(0, &digOut2); //Clutch on
			board->setDigital(2, digOut2); //Trigger it
			usleep(10000); //10Msec sleep to allow clutch to pop
		//Shift
			bitset(2, &digOut2); //We throw the down shift
			board->setDigital(2, digOut2); //Trigger it
			usleep(10000); // 10Msec sleep to allow shifter to occur
		//Release
			bitunset(2, &digOut2); //Now we release the shift
			board->setDigital(2, digOut2); //Trigger it
			usleep(10000); // 10Msec sleep to allow shifter to rebound

			bitunset(0, &digOut2); //Clutch release
		}


	//Manual clutch routine
		if(bitof(7, digIn1) && bitof(7, board->getDigital(0)))
			bitset(0, &digOut2);
		else
			bitunset(0, &digOut2);

}


/******Super Safe Neutral Finding Routine******/
inline void Car::ensureNeutral()
{
//Clutch
	bitset(0, &digOut2); //Clutch on
	board->setDigital(2, digOut2); //Trigger it
	usleep(500000); // 1/2 sec sleep to allow clutch to pop
	do
	{
		if( bitof(4, digIn2) ) //Neutral Indicator
				break;
	//Shift
		bitset(2, &digOut2); //We throw the down shift
		board->setDigital(2, digOut2); //Trigger it
		usleep(500000); // 1/2 sec sleep to allow shift to occur
	//Release
		bitunset(2, &digOut2); //Now we release the shift
		board->setDigital(2, digOut2); //Trigger it
		usleep(500000); // 1/2 sec sleep to allow shift to rebound

	}while(true);

//Clutch Release
	bitunset(0, &digOut2); //Clutch release
	board->setDigital(2, digOut2); //Trigger it
}

inline void Car::throttleCalc(float percentage)
{
//Before setting new throttle output, store previous value for hysterisis
	throttleOutPrevious = throttleOut;

	const unsigned char max_duty = 40; // using this so that we can adjust our endpoints of servo travel
	const unsigned char min_duty = 22;
	throttleOut = (unsigned char)(((float)(max_duty - min_duty) * percentage) + (float)min_duty);
	if(throttleOut < min_duty)
			throttleOut = min_duty;
	if(throttleOut > max_duty)
			throttleOut = max_duty;
}

