
#include "vcmdas.h"
#include "time.h"
#include "python_io.h"
#include "string.h"

int main(int argc, char *argv[]){

	PythonIO* board= new PythonIO();
	VCMDAS* das = new VCMDAS();
/*	board->initDigitalIO(0xFFFFFF00);

	for(unsigned char i=0; i<=255; i++)
	{
			if(board->getDigital(1) < 255)
			{
				unsigned char bar = (board->getDigital(1));
				int foo = 1;
				for(int j=0; j<=7;j++)
				{
						if(bitof(j, bar))
							foo *=2;
				}
				usleep(1000 * foo);
			}
			else
				usleep(1000000);
			board->setDigital(0,i);
	}
	do
	{
			board->setDigital(0, board->getDigital(1));
	}while(true);*/

	unsigned char dig1 = board->getDigital(0);
	board->initDigitalIO(0xFFFF00FF);
	if(dig1 != 255)
		return 0;
	
	int oldval=0;
	board->initPWM();
	for(unsigned char i=0; i <=255; i++)
	{
			double value = das->getAnalog(0);
			int outval = (int)(value * (19./5.)) + 22;
			if(outval < 22)
				outval = 22;
			if(outval > 40)
				outval = 40;
			if(outval != oldval)
			{
				board->setPWMDuty(outval);
				//if( abs(oldval-outval) > 2)
				//	printf("Old=%d, New=%d, Value=%f\n", oldval, outval, value);
				oldval = outval;
			}
//		usleep(10);
		board->setDigital(1, (outval - 21));
		dig1 = board->getDigital(0);
		if(dig1 != 255)
		{
			dig1 = board->getDigital(0);
			if(dig1 != 255)
			{
				board->setDigital(1, dig1);
				return 0;
			}
		}
		//printf("Just set duty to %d\n", outval);
	}while(true);

	return 0;

}
