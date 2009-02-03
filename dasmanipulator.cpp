

#include <stdio.h>
#include <cstring>
#include "vcmdas.h"
#include <math.h>

int main(){
	char function [50];
	int arg1;
	int arg2;
	VCMDAS* das = new VCMDAS(0x0300,0x0000);
	das->enableEEPROMWrite();
	
	do{
		printf("\n\nPlease enter your desired action:\n");
		scanf("%s", &function);
//		printf("FUNCTION = \"%s\"\n", function);
//		printf("ARG1 = %d\n", arg1);
//		printf("ARG2 = %d\n", arg2);
		if(strcmp(function,"AI") == 0 || strcmp(function, "ai") == 0 || strcmp(function, "getAnalog") == 0)
		{
			printf("What channel?");
			if(scanf("%d", &arg1) == 0)
			{
				printf("Bad argument...\n");
				continue;
			}
			int avgval = (int)((das->getAnalog(arg1) + das->getAnalog(arg1) + das->getAnalog(arg1) + das->getAnalog(arg1) + das->getAnalog(arg1) + das->getAnalog(arg1) + das->getAnalog(arg1) + das->getAnalog(arg1))/8);
			printf("Value is %d\n", avgval);
			//printf("Value is %d\n", das->getAnalog(arg1));
			continue;
		}
		if(strcmp(function, "AO") == 0 || strcmp(function, "ao") == 0 || strcmp(function, "setAnalog") == 0)
		{
			printf("What channel?");
			if(scanf("%d", &arg1) == 0)
			{
				printf("Bad argument...\n");
				continue;
			}
			printf("What output data?");
			if(scanf("%d", &arg2) == 0)
			{
				printf("Bad argument...\n");
				continue;
			}
			das->setAnalog(arg1,arg2);
			continue;
		}
		if(strcmp(function, "DI") == 0 || strcmp(function, "di") == 0 || strcmp(function, "getDigital") == 0)
		{
			printf("No extra args needed...\n");
			printf("Digital port = %d\n",das->getDigital());
			continue;
		}
		if(strcmp(function, "DO") == 0 || strcmp(function, "do") == 0 || strcmp(function, "setDigital") == 0)
		{
			printf("What output data?");
			if(scanf("%d", &arg1) == 0)
			{
				printf("Bad argument...\n");
				continue;
			}
			das->setDigital(arg1);
			continue;
		}
		if(strcmp(function, "writeEEPROM") == 0)
		{
			printf("What address?");
			if(scanf("%d", &arg1) == 0)
			{
				printf("Bad argument...\n");
				continue;
			}
			printf("What data?");
			if(scanf("%d", &arg2) == 0)
			{
				printf("Bad argument...\n");
				continue;
			}
			printf("Are you REALLY sure you want to write the value %d to %d?\n",arg2, arg1);
			scanf("%s", &function);
			if(strcmp(function, "y") == 0 || strcmp(function, "Y") == 0 || strcmp(function, "yes") == 0)
			{
					printf("writing data...\n");
					das->writeEEPROM(arg1, arg2);
			}
			else
			{
					printf("not writing data\n");
			}
			continue;
		}
		if(strcmp(function, "readEEPROM") == 0 || strcmp(function, "read") == 0)
		{
			printf("What chip address should I read from? (0-63):");
			if(scanf("%d", &arg1) == 0)
			{
				printf("Bad argument...\n");
				continue;
			}
			printf("Data at %d = %d\n", arg1, das->readEEPROM(arg1));
			continue;
		}

		if(strcmp(function, "writeDPot") == 0 || strcmp(function, "dpot") == 0)
		{
			printf("What pot should I set?\n\t0 = Analog Input Offset\n\t1 = Analog Output (channel 0) Gain\n\t2 = Analog Input Gain\n\t3 = Analog Output (channel 1) Gain\n");
			if(scanf("%d", &arg1) == 0)
			{
				printf("Bad argument...\n");
				continue;
			}
			printf("What value?\n");
			if(scanf("%d", &arg2) == 0)
			{
				printf("Bad argument...\n");
				continue;
			}
			das->writeDPot(arg1, arg2);
			continue;
		}
		if(strcmp(function, "inpu") == 0)
		{
				printf("This function will set the analog input offset voltage - Please connect pin 1 to pin 3 on header J2. When finished press any key to begin\n");

		}
		if(strcmp(function, "outputgain") == 0)
		{
				printf("This function will set the output gain. Please be sure that the loopback jumpers are connected\n");
				das->writeDPot(1, 0);
				das->writeDPot(3, 0);

				das->setAnalog(0, 4095);
				unsigned char value = 0;
				do{
					int tmpval = (int)((das->getAnalog(14) + das->getAnalog(14) + das->getAnalog(14) + das->getAnalog(14))/4);
					if( (32767 - tmpval) < 10)
							break;
					value++;
					das->writeDPot(1, value);
				}while(value < 250);
				printf("Channel 0 gain is %d\n",value);
				value = 0;
				do{
					int tmpval = (int)((das->getAnalog(15) + das->getAnalog(15) + das->getAnalog(15) + das->getAnalog(15))/4);
					if( (32767 - tmpval) < 10)
							break;
					value++;
					das->writeDPot(3, value);
				}while(value < 250);
				printf("Channel 1 gain is %d\n",value);
		}
		if(strcmp(function, "inputgaincalib") == 0)
		{ 
			int input_channel;
			int target_value;
			printf("This takes two arguments, they are the mathematically calculated value that the converter should read and the channel you are reading (to set it to that number) while you modulate the gain\n");
			printf("(Attach a  voltage source to some input and we are going to change the gain until it reads the correct value according to the external voltage measuring device)");
			printf("What shall I use as the channel to use for adjusting the gain?\n");
			if(scanf("%d", &input_channel) == 0)
			{
				printf("Bad argument...\n");
				continue;
			}
			printf("What is the value I'm trying to make this channel? (input voltage/(20/65536))");
			if(scanf("%d", &target_value) == 0)
			{
				printf("Bad argument...\n");
				continue;
			}

			unsigned char pot_val = 0x80;
			int tmp_reading;
			do
			{
				printf("pot_val is %d\n", pot_val);
				tmp_reading = das->getAnalog(input_channel);
				if( tmp_reading < target_value && pot_val !=1)
				{
						pot_val--;
						das->writeDPot(2, pot_val);
				}
				if(tmp_reading > target_value && pot_val !=254)
				{
						pot_val++;
						das->writeDPot(2, pot_val);
				}
				if( abs(tmp_reading - target_value) < 10 )
				{
					//begin osscilation around value
					
					int previous_val = 0;
					int average_val = 0;
					do
					{
						//get average of 5 values
						for(int i=0; i<5; i++)
							average_val += das->getAnalog(input_channel);
						average_val /= 5;
						printf("average_val was %d\n", average_val);
						if( abs(average_val - target_value) < 8)
								break;
						if( average_val < target_value && pot_val !=1)
						{
							if(previous_val  > target_value)
								break;
							pot_val--;
							das->writeDPot(2, pot_val);
						}
						if(average_val > target_value && pot_val !=254)
						{
							if(previous_val !=0 && previous_val < target_value)
								break;
							pot_val++;
							das->writeDPot(2, pot_val);
						}
						previous_val = average_val;
					}while(true);
					break;
				}
			}while(pot_val != 1 && pot_val !=254);
			continue;
		}

		if(strcmp(function, "exit") == 0)
				break;
		else
		{
				printf("Unrecognized function, possibilities include:\n");
				printf("\tAI == ai == getAnalog\n");
				printf("\tAO == ao == setAnalog\n");
				printf("\tDI == di == getDigital\n");
				printf("\tDO == do == setDigital\n");
				printf("\twriteEEPROM\n");
				printf("\tread == readEEPROM\n");
				printf("\tdpot == writeDPot\n");
				printf("\tinputgaincalib\n");
				printf("\toutputgain\n");
				printf("\texit\n");
		}
	}while(true);
		
	return 0;
}
