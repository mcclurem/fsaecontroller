
#include "vcmdas.h"
#include "time.h"
#include "python_io.h"
#include "string.h"

int main(int argc, char *argv[]){

	PythonIO* board= new PythonIO();
	board->initPWM();
	VCMDAS* das = new VCMDAS();
	do
	{
		double value = das->getAnalog(0);
		int outval = (int)(value * (18./5.)) + 22;
		if(outval < 22)
			outval = 22;
		if(outval > 39)
			outval = 39;
		board->setPWMDuty(outval);
		printf("Just set duty to %d\n", outval);
	}while(true);

	return 0;

}
