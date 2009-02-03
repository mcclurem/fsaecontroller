

#include <stdio.h>
#include "vcmdas.h"
#include "mytime.h"


//The goal of these macros is to have them be boolean equivalents
//in order to test the individual inputs
#define GAS_ENABLE (digital_In & 0x01)
#define ELECTRIC_ENABLE ((digital_In > 1) & 0x01)
#define HYBRID_ENABLE (GAS_ENABLE & ELECTRIC_ENABLE)

#define READY (digital_Out1 = digital_Out1 | 0x80)
#define FAULT (digital_Out1 = digital_Out1 | 0xC0)


int main(){
//We need to set up some globals for storing info
//I believe pointers would be bad because we want fast access times
	
	short int pedal_Position;
	int engine_RPM;
	float gear_Ratio;
	int array_Voltage;
	char digital_In;
	char digital_Out1;
	char digital_Out2;
	int throttle_Position; //A number between 0 and 100

	Init_IO(); //We need to perform a handful of initializations
//the infinite loop
	READY;
	do{
		//Begin inputs

		pedal_Position = DAS_Get_Analog(0); 
		digital_In = Get_Digital();

		//End inputs

		//Begin math block
		if( !HYBRID_ENABLE ){
				if(ELECTRIC_ENABLE){
						//We throw the clutch on the gas and use the pedal position to control electric motor

						//pull clutch or engage neutral gas

						//Send CAN message -  Torque = pedal-position
					//gas servo closed
					//electric throttle directly prop pedal-position
					//
				}
				if(GAS_ENABLE){
						//We just use the pedal position to control throttle position and set electric to neutral
						
						//Send CAN message neutral
						
						//Throttle = pedal-position
					//electric throttle off
					//regen most DEFINITELY off
				}
				if(!GAS_ENABLE && !ELECTRIC_ENABLE){
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
				if(bank_voltage > 72)
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
				}



		}
		//End math block
		
		//Begin Outputs
		
		//End Outputs
	}while(true);
//end infinite loop - wait isn't that an oxy-moron?

}
