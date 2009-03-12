

#include <stdio.h>
#include "vcmdas.h"
#include "python_io.h"
#include "mytime.h"


//The goal of these macros is to have them be boolean equivalents
//in order to test the individual inputs
#define GAS_ENABLED (digital_in1 & 0x01)
#define ELECTRIC_ENABLED ((digital_in1 > 1) & 0x01)
#define HYBRID_ENABLED (GAS_ENABLED & ELECTRIC_ENABLED)

#define READY (digital_out1 = digital_out1 | 0x80)
#define FAULT (digital_out1 = digital_out1 | 0xC0)

/*
 * This is just a function to return a PWM duty from a percentage value
 */
inline unsigned char ThrottleCalc(double percentage)
{
	const double max_duty = 40; // using this so that we can adjust our endpoints of servo travel
	const double min_duty = 22;
	unsigned char retval;
	retval = (unsigned char)(((max_duty - min_duty) * percentage) + min_duty);
	if(retval < min_duty)
			retval = min_duty;
	if(retval > max_duty)
			retval = max_duty;
	return retval;
};

int main(){
//We need to set up some globals for storing info
	double pedal_position; //Normalized number from 0-1
	int engine_RPM;
	float gear_ratio;
	int array_voltage;
	unsigned char digital_in1; //Bits 0-7
	unsigned char digital_in2; //Bits 24-31
	unsigned char digital_out1; //Bits 8-15
	unsigned char digital_out2; //Bits 16-23
	unsigned char throttle_pos = 0; //The PWM duty cycle
	
	PythonIO * board = new PythonIO();
	VCMDAS * das = new VCMDAS();
	board->initDigitalIO(0xFF0000FF);

//the infinite loop
	READY;
	do{
		//Begin inputs
//***************************************************
		pedal_position = (das->getAnalog(0))/5.; //NB: assumes DAS is set in +-5V mode
		digital_in1 = board->getDigital(0);
		digital_in2 = board->getDigital(3);

		//End inputs

		//Begin math block
		if( !HYBRID_ENABLED ){
				if(ELECTRIC_ENABLED){
						//We throw the clutch on the gas and use the pedal position to control electric motor

						//pull clutch or engage neutral gas

						//Send CAN message -  Torque = pedal-position
					//gas servo closed
					//electric throttle directly prop pedal-position
					//
				}
				if(GAS_ENABLED){
						//We just use the pedal position to control throttle position and set electric to neutral

						//We want to avoid constantly reseting the PWM:
						if(ThrottleCalc(pedal_position) != throttle_pos)
						{
								throttle_pos = ThrottleCalc(pedal_position);
								board->setPWMDuty(throttle_pos);
						}
						//Force the electric motor to free-wheel:
						digital_out1 &= (0x0F|0x80|0x10); //should yield binary: 10011111
						
						//Throttle = pedal-position
					//electric throttle off
					//regen most DEFINITELY off
				}
				if(!GAS_ENABLED && !ELECTRIC_ENABLED){
					//gas servo closed
					//electric throttle off
					//regen off
					//FAULT
				}
		}else{	//Our hybrid math goes here
		
				//Our main function in here is %pedal = (%servo*gas_torque + %electric_throttle/regen*electric_torque)/(gas_torque + electric_torque)
				//unknowns are %gas_torque & %electric throttle
				//assisting equations:
					//if(bank_voltage > 72v){electric throttle is >=0;} //gas does all the work unhindered and electric can only help
					//else {electric throttle = 
				/*if(bank_voltage > 72)
				{
						//something a little trickier that does some kind of down-ramping
				}else
				{//We got lots o' space available in the bank, so charge her on up
						if((GAS_TORQUE - ELECTRIC_TORQUE) > request)
						{//saturate the regen - 100%regen + request is still less than 100%gas
								ELECTRIC_OUT = -100;
								GAS_OUT = ((request * NET_TORQUE) + ELECTRIC_TORQUE)/NET_TORQUE;	//Any _OUT var is 0-100; request is a percent as well
						}else
						{//looks like we need more than gasengine - fullregen so we need to regen a little less in order to allow us to get the requested out
								GAS_OUT = 100;
								ELECTRIC_OUT = ((request * NET_TORQUE) - GAS_TORQUE)/NET_TORQUE;
						}
				}*/



		}
		//End math block
		
		//Begin Outputs
		
		board->setDigital(1, digital_out1);
		board->setDigital(2, digital_out2);

		//End Outputs
	}while(true);
//end infinite loop - wait isn't that an oxy-moron?

}
