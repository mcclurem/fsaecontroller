
#include "car.h"

#define MODE_BIT_1 (bitof(5, digIn1))
#define MODE_BIT_2 (bitof(6, digIn1))
#define MODE_BIT_3 (bitof(7, digIn1))

#define MODE_0 (MODE_BIT_1 && MODE_BIT_2 && MODE_BIT_3)
#define MODE_1 (!MODE_BIT_1 && MODE_BIT_2 && MODE_BIT_3)
#define MODE_2 (MODE_BIT_1 && !MODE_BIT_2 && MODE_BIT_3)
#define MODE_3 (!MODE_BIT_1 && !MODE_BIT_2 && MODE_BIT_3)
#define MODE_4 (MODE_BIT_1 && MODE_BIT_2 && !MODE_BIT_3)

#define ESTOP (bitof(0, digIn1))

#define BANK_OFF bitunset(3, &digOut1)
#define BANK_ON bitset(3, &digOut1)

#define MC_OFF bitunset(4, &digOut1)
#define MC_ON bitset(4, &digOut1)

#define IGN_OFF bitunset(5, &digOut1)
#define IGN_ON bitset(5, &digOut1)

#define FAN_OFF bitunset(6, &digOut2)
#define FAN_ON bitset(6, &digOut2)

#define STARTER_OFF bitunset(7, &digOut2)
#define STARTER_ON bitset(7, &digOut2)

#define WDOG_OFF bitunset(0, &digOut2)
#define WDOG_ON bitset(0, &digOut2)

#define LED1_ON bitset(1, &digOut2)
#define LED1_OFF bitunset(1, &digOut2)

#define LED2_ON bitset(2, &digOut2)
#define LED2_OFF bitunset(2, &digOut2)

#define LED3_ON bitset(3, &digOut2)
#define LED3_OFF bitunset(3, &digOut2)

#define LED4_ON bitset(4, &digOut2)
#define LED4_OFF bitunset(4, &digOut2)

#define LED5_ON bitset(5, &digOut2)
#define LED5_OFF bitunset(5, &digOut2)


Car::Car()
{ 
		das1 = new VCMDAS();
		das2 = new VCMDAS(0x310);
		pcm = new PCM(0x200);
		//Set das1 up as all outputs
		das1->setDigitalDirection(2, false);
		throttleCalc(0.);
} 



void Car::run()
{ 
	do
  	{
		toggleWatchDog();
		inputQuery();//Query the ins to simplify each loop
		
		//Hybrid
		if(MODE_1)
		{
			//need to set some state variables on first entry:
			if( mode != HYBRID )
			{
				LED1_OFF;
				LED2_ON;
				LED3_OFF;
				LED4_OFF;
				MC_ON;
				BANK_ON;
				IGN_ON;
			//Set the Mode state variable
				mode = HYBRID;
			}
				hybridLoop();
		}


		//Gas
		if(MODE_0)
		{
			//need to set some state variables on first entry:
			if( mode != GAS )
			{
				LED1_ON;
				LED2_OFF;
				LED3_OFF;
				LED4_OFF;
				MC_OFF;
				BANK_OFF;
			//Set the Mode state variable
				mode = GAS;
 			}
			gasLoop();

 		}
		

		//Electric
		if(MODE_2)
		{
			//need to set some state variables on first entry:
			if( mode != ELECTRIC )
			{
				LED1_OFF;
				LED2_OFF;
				LED3_ON;
				LED4_OFF;
				MC_ON;
				BANK_ON;
				IGN_OFF;
				FAN_OFF;
				throttleCalc(0.); //Close the throttle
			//Set the Mode state variable
				mode = ELECTRIC;
			}
			electricLoop();
		}


		writeOutputs();
	}while(true);
}



void Car::gasLoop()
{ 
	if( bitof(1, digIn2))
		IGN_ON;
	else
		IGN_OFF;

//	throttleCalc(pedalPosition);

//Fans with some hysterisis
//	fanHandler();
//Handle shifting
//	shiftHandler();

//If tach is less than 500RPM then engine is not running
//This block is a very simple starting routine, setting this particular
//RPM value accurately is going to be important
	STARTER_OFF;
	if(engineRPM < 500)
		startGas();

}


void Car::electricLoop()
{
	if(bankVoltage < 30.)
	{
		//LOW BANK!!!!

	}
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
/*
	float currentEngineTorque = gasMap->lookup(engineRPM, throttlePosition);
	float currentMotorTorque = electricMap->lookup(engineRPM, electricThrottlePercentage);
	//How does this shit work?

	shiftHandler();
	fanHandler();
	*/
}



void Car::inputQuery()
{
	capWarn = pcm->getDigital(0);
	capStop = pcm->getDigital(1);
	//fetch port C0
	digIn1 = pcm->getDigital(2);
	//fetch port A1
	digIn2 = pcm->getDigital(3);


	//Now we go ahead and pull all the VCMDas analog inputs
	pedalPosition = ((float)das1->getRawAnalog(9))/32767.;
	brakePosition = ((float)das1->getRawAnalog(8))/32767.;

	//These all need calibrating
	throttlePosition = ((float)das1->getRawAnalog(15))/32767.;
	bankVoltage = ((float)das1->getRawAnalog(11))/32767.;
	shiftPressure = ((float)das1->getRawAnalog(10))/32767.;
/*
	cap0Temp = ((float)das->getRawAnalog(6))/32767.;
	cap1Temp = ((float)das->getRawAnalog(7))/32767.;
	cap2Temp = ((float)das->getRawAnalog(8))/32767.;
	cap3Temp = ((float)das->getRawAnalog(9))/32767.;
	cap4Temp = ((float)das->getRawAnalog(10))/32767.;
	cap5Temp = ((float)das->getRawAnalog(11))/32767.;
*/

	waterTemp = ((float)das2->getRawAnalog(0))/32767.;
	
	frontLeftRPM = ((float)das2->getRawAnalog(1))/32767.;
	frontRightRPM = ((float)das2->getRawAnalog(2))/32767.;
	rearLeftRPM = ((float)das2->getRawAnalog(3))/32767.;
	rearRightRPM = ((float)das2->getRawAnalog(4))/32767.;
	brakeFrontPressure = ((float)das2->getRawAnalog(5))/32767.;
	brakeRearPressure = ((float)das2->getRawAnalog(6))/32767.;

	engineRPM = ((float)das2->getRawAnalog(7))/32767.;
}



void Car::writeOutputs()
{
	//Write our two python digital outs
//	board->setDigital(1, digOut1);
//	board->setDigital(2, digOut2);

	writeDigital();
/*
	//Get our cap status
	word dasIn;
	dasIn.value = das->getDigital();
	capWarn = dasIn.lsb;
	capStop = dasIn.msb;
*/
	//Write out to the electric
	unsigned short throt = (unsigned short)(electricThrottlePercentage * 4095.);
	unsigned short regen = (unsigned short)(electricRegenPercentage * 4095.);
	das2->setAnalog(0, throt);
	das2->setAnalog(1, regen);
}

inline void Car::writeDigital()
{
	word digout;
	digout.lsb = digOut1;
	digout.msb = digOut2;
	das1->setDigital(digout.value);
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

/* COMMENTED TO REDUCE DEBUGGING HEADACHE
	//upshift routine
		if(bitof(5, digIn1) && bitof(5, board->getDigital(0)) && !upShifted)
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
			upShifted = true;
		}	


	//downshift routine
		if(bitof(6, digIn1) && bitof(6, board->getDigital(0)) && !downShifted)
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
			downShifted = true;
		}

	//Manual clutch routine
		if(bitof(7, digIn1) && bitof(7, board->getDigital(0)))
			bitset(0, &digOut2);
		else
			bitunset(0, &digOut2);

		if(!bitof(5, digIn1))
			upShifted = false;
		if(!bitof(6, digIn1))
			downShifted = false;

*/

}

void Car::startGas()
{
	if( bitof(2, digIn2))
		STARTER_ON;
	return;

	int timeout;
	//Starter is being pressed? (bitof(2, digIn1)
	if( !bitof(2, digIn1) )
			return;
	//maximum starting time before fault = timeout * pausetime = 3secs
	for(timeout = 300; (timeout > 0) && (engineRPM < 1500); timeout--)
	{
		ensureNeutral();
		//Allow the throttle to be at most 3% open
		if(!(throttlePosition < 0.03))
		{
			throttleCalc(0.);
			writeOutputs();
			usleep(10000);//Pause 10Msec to allow for throttle to close
		}
		STARTER_ON;
		bitset(7, &digOut2); //Starter relay on
		writeDigital();
		usleep(10000); //10Msec pause

	}
	STARTER_OFF; //Starter relay off
	writeDigital(); //Trigger relay off
	if( timeout > 1 )
		bitunset(4, &digOut1);
	else
		bitset(4, &digOut1);

/*
 * Crappier version
	if( engineRPM < 500. )
	{
		//very simple starting routine
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
*/
}

inline void Car::toggleWatchDog()
{
	if(dog_stat)
	{
		WDOG_OFF;
		dog_stat = false;
	}
	else
	{
		WDOG_ON;
		dog_stat = true;
	} 
}


/******Super Safe Neutral Finding Routine******/
inline void Car::ensureNeutral()
{
	if( bitof(4, digIn2) ) //Neutral Indicator
			return;
//Clutch
	bitset(0, &digOut2); //Clutch on
	writeDigital(); //Trigger it
	usleep(500000); // 1/2 sec sleep to allow clutch to pop
	do
	{
		if( bitof(4, digIn2) ) //Neutral Indicator
				break;
	//Shift
		bitset(2, &digOut2); //We throw the down shift
		writeDigital(); //Trigger it
		usleep(500000); // 1/2 sec sleep to allow shift to occur
	//Release
		bitunset(2, &digOut2); //Now we release the shift
		writeDigital(); //Trigger it
		usleep(500000); // 1/2 sec sleep to allow shift to rebound

	}while(true);

//Clutch Release
	bitunset(0, &digOut2); //Clutch release
	writeDigital(); //Trigger it
}

inline void Car::throttleCalc(float percentage)
{	
/*
//Before setting new throttle output, store previous value for hysterisis
	throttleOutPrevious = throttleOut;

	const unsigned char max_duty = 40; // using this so that we can adjust our endpoints of servo travel
	const unsigned char min_duty = 22;
	throttleOut = (unsigned char)(((float)(max_duty - min_duty) * percentage) + (float)min_duty);
	if(throttleOut < min_duty)
			throttleOut = min_duty;
	if(throttleOut > max_duty)
			throttleOut = max_duty;
*/
}
