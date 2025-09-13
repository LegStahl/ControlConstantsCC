
#include "ControlConstantsOverUdp.h"
int check = 0;


int main(int argc, char *argv[])
{
	#ifdef WIN32
	check = 32;	
	#endif
	ControlConstants &temp = ControlConstants::get_ControlConstants();
	ControlConstants::get_ControlConstants();
	request_t t;
	//temp.write_request(t);
	time_t rt = time(0);
	std::cout << "Hello World! " << rt << std::endl;
	return 0;
}
