
#include "ControlConstantsOverUdp.h"
int check = 0;
#define TEST_IP                0xc0a8c915  //192.168.201.21

int main(int argc, char *argv[])
{
	#ifdef WIN32
	check = 32;	
	#endif
	ControlConstants &temp = ControlConstants::get_ControlConstants();
	ControlConstants::get_ControlConstants();
	request_t t;
	uint8_t input;
	float d = 0;
	while(1)
	{
		std::cout << "Program has started!" << sizeof(SOCKET) << std::endl;
		std::cin >> input;
		std::cout << "Writen was " << input <<  std::endl;
		if(input == '0')
		{
			std::cout << "Read request will be sent" << std::endl;
			temp.make_read_request(&t, TEST_IP, TEST_ADDR_COMMAND);
			if(temp.do_request(READ_REQ, &t) == SUCCESS)
			{
				std::cout << "DATA IN MAIN" << std::endl;
			}
			
		}
		if(input == '1')
		{
			d++;
			std::cout << "Write request will be sent" << std::endl;
			temp.make_write_request(&t, TEST_IP, TEST_ADDR_COMMAND_2,(uint8_t*)&d, sizeof(d) );
			if(temp.do_request(WRITE_REQ, &t) == SUCCESS)
			{
				std::cout << "DATA IN MAIN" << std::endl;
			}
		}
		
	}
	//temp.write_request(t);
	time_t rt = time(0);
	std::cout << "Hello World! " << rt << std::endl;
	return 0;
}
