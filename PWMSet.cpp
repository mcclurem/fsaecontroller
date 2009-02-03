
#include "vcmdas.h"
#include "time.h"
#include "python_io.h"
#include "string.h"

int main(int argc, char *argv[]){

	//printf("Argc = %d\n", argc);

	if( argc == 2 )
	{
			unsigned char value = atoi(argv[1]);

			printf("Setting the Duty Cycle to %d ...\n", value);

			PythonIO* board= new PythonIO();
			board->initPWM();
			board->setPWMDuty(value);
			printf("Done setting PWM\n\nNow testing tach in 0 ...\n");
			float freq = 1. / ((board->getTach(0) * 0.00001111) / 2. );
			int rpm = freq * 30;
			printf("RPM is: %d\n", rpm);
			printf("Frequency is: %f\n", freq);
	}

	return 0;

}
