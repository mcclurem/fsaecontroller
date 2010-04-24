
#include "definitions.h"
#include "table.h"
#include "vcmdas.h"
#include "python_io.h"



class Car
{
	public:
		Car();
		void run();

		enum modeType { HYBRID, GAS, ELECTRIC, FAULT };
	private:

		void gasLoop();
		void electricLoop();
		void hybridLoop();

		void inputQuery();
		void writeOutputs();

		void startGas();
		inline void fanHandler();
		inline void shiftHandler();
		inline void ensureNeutral();
		inline void throttleCalc(float percentage);
		
		VCMDAS *das1;
		VCMDAS *das2;
		PythonIO *board;

		Table *gasMap;
		Table *electricMap;

		unsigned char digIn1;
		unsigned char digIn2;
		unsigned char digOut1;
		unsigned char digOut2;
		unsigned char capWarn;
		unsigned char capStop;
		unsigned char throttleOutPrevious;
		unsigned char throttleOut;

		modeType mode;

		float electricRegenPercentage;
		float electricThrottlePercentage;

		float pedalPosition;
		float brakePosition;
		float brakeFrontPressure;
		float brakeRearPressure;
		float waterTemp;
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

	//Two state variables used to prevent accidental double shifts
		bool upshifted;
		bool downshifted;

};
