
#include "vcmdas.h"
#include "time.h"
#include "python_io.h"
#include "string.h"

int main(int argc, char *argv[]){

	PythonIO* board= new PythonIO();
	VCMDAS* das = new VCMDAS();
	unsigned char dig1 = board->getDigital(0);
	if(!(dig1 & 0x01))
		return 0;
	
	board->initPWM();
	do
	{
		double value = das->getAnalog(0);
		int outval = (int)(value * (18./5.)) + 22;
		if(outval < 22)
			outval = 22;
		if(outval > 39)
			outval = 39;
		board->setPWMDuty(outval);
		dig1 = board->getDigital(0);
		if(!(dig1 & 0x01))
				return 0;
		//printf("Just set duty to %d\n", outval);
	}while(true);

	return 0;

}
