
#include "ControlConstantsOverUdp.h"
int check = 0;
#define TEST_IP                0xc0a8c915  //192.168.201.21

union FLOATD
{
	float data;
	uint32_t hex;
};

int main(int argc, char* argv[])
{
#ifdef WIN32
	check = 32;
#endif
	{
		ControlConstants* temp = ControlConstants::get_ControlConstants();
		
		request_t t;
	}
	ControlConstants* temp = ControlConstants::get_ControlConstants();
	
	request_t t;
	uint8_t input;
	float d = 0;
	
	std::cout << "Hello" << std::endl;
	while(1)
	{
	
		std::cin >> input;
		std::cout << "Writen was " << input <<  std::endl;
		if(input == '0')
		{
			std::cout << "Read request will be sent" << std::endl;
			temp->make_read_request(&t, TEST_IP, TEST_ADDR_COMMAND);
			if(temp->do_request(READ_REQ, &t) == SUCCESS)
			{
				float test_data = 0; 
				int test_data_in_int = 0;
				int j = 0;
				FLOATD test;
				for (int i = 4 -1; i  >= 0; i--)
				{					
					test_data_in_int = (int)test_data_in_int | (t.param.data[i] << (j * 8));
					j++;
				}
			
				test.hex = test_data_in_int;
				std::cout << "DATA IN MAIN data " << test.data << "\n";
				std::cout << "DATA IN MAIN hex " << test.hex << "\n";
				std::cout << "DATA IN MAIN te1 " << test_data_in_int << "\n";
				std::cout << "DATA IN MAIN d " << d << "\n";
				std::cout << "DATA IN MAIN te " << test_data << "\n";
			
			}
			
		}
		if(input == '1')
		{
			d++;
			std::cout << "Write request will be sent" << std::endl;
			temp->make_write_request(&t, TEST_IP, TEST_ADDR_COMMAND_2,(uint8_t*)&d, sizeof(d) );
			if(temp->do_request(WRITE_REQ, &t) == SUCCESS)
			{
			
				std::cout << "DATA IN MAIN " << std::endl;
			}
		}
		if (input == '2')
		{
			temp->stop_socket_and_thread();
		}
		if (input == '3')
		{
			temp = ControlConstants::get_ControlConstants();
		}
		if(input == '4')
		{
			break;
		}
		
	}

	return 0;
}
