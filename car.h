

class Car
{
	public:
		Car();
	private:
		VCMDAS das;
		PythonIO board;

		unsigned char digIn1;
		unsigned char digIn2;
		unsigned char digOut1;
		unsigned char digOut2;
		unsigned char capWarn;
		unsigned char capStop;

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
