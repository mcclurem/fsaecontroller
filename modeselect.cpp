
#include "vcmdas.h"




int main(){
	das = new VCMDAS(0x300);
	unsigned short digin = das->getDigital();
	if(bitof(1,digin) and !bitof(2,digin))
		return 0;
	else
		return 1;

}
