
#include "pcm.h"
#include "definitions.h"



int main(){
	PCM * pcm = new PCM(0x200);
	unsigned short digin = pcm->getDigital(5);
	if(bitof(7,digin))
	{
		return 0;
	}
	else
	{
		if(bitof(6, digin))
		{
			return 2;
		}
		else
		{
			return 1;
		}
	}

}
