
#include "definitions.h"
#include "vcmdas.h"
#include "python_io.h"

class Car
{
	public:
		Car();
		void run();
	private:

		void gasLoop();
		void electricLoop();
		void hybridLoop();

		void inputQuery();
		void writeOutputs();
		
		VCMDAS das;
		PythonIO board;

		Table gasMap;
		Table electricMap;

		unsigned char digIn1;
		unsigned char digIn2;
		unsigned char digOut1;
		unsigned char digOut2;
		unsigned char capWarn;
		unsigned char capStop;
		unsigned char throttleOutPrevious;
		unsigned char throttleOut;

		float electricRegenPercentage;
		float electricThrottlePercentage;

		float pedalPosition;
		float brakePosition;
		float engineTemp;
		float bankVoltage;
		float throttlePosition;
		float shiftPressure;
		float cap0Temp;
		float cap1Temp;
		float cap2Temp;
		float cap3Temp;
		float cap4Temp;
		float cap5Temp;
		float cap6Temp;
		float frontLeftRPM;
		float frontRightRPM;
		float rearLeftRPM;
		float rearRightRPM;
		float engineRPM;

};
