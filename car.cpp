
#include "car.h"


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

Car::Car()
{
		das = VCMDAS();
		board = PythonIO();
		
		das->setDigitalDirection(2, false);
		setPWMDuty(ThrottleCalc(0.));
		board->initDigitalIO(0xFF0000FF);

}
