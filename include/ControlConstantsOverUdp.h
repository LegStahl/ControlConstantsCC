#ifndef  CONTROLCONSTANTSOVERUDP
#define  CONTROLCONSTANTSOVERUDP
//Для кроссплатформенности добавляем проверки, при использовании Qt можно этого избежать
//так как есть QNetwork
#include <iostream> 
#include <thread>
#include <atomic>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <map>
#include <mutex>
#include <iomanip>
#include <memory>
#include <cstring>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
    #define CLOSESOCK closesocket
    typedef int socklen_t;
	
#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #define CLOSESOCK close
#endif
//Possbile ports for reading and writing, programm is tested on one machine
#define PORTLISTEN             32766
#define PORTSEND               8888

//There is an array of possible addresses by index from define you can get required address of command
#define COUNT_OF_COMMAND       4//Current quantity of commands
#define TEST_ADDR_COMMAND      0//For reading addr
#define TEST_ADDR_COMMAND_1    1//For reading addr
#define TEST_ADDR_COMMAND_2    2//For writing addr
#define TEST_ADDR_COMMAND_3    3//For writing addr
//
#define LENGTH_DATA_PART       8
#define LENGTH_OF_BUFFER       1024

#define BROADCAST_IP           "127.0.0.1" //"255.255.255.255"

//Типы запросов
#define WRITE_REQ               1
#define READ_REQ                2
//

#define SUCCESS                 0
#define FAILURE                 1


	
#pragma pack(push, 1)														//Disable padding
typedef struct  
{
	uint16_t address;
	uint8_t data[LENGTH_DATA_PART];											//When it is reading request this array must be filled with zeros
}param_t;


typedef struct  
{
	uint16_t header;
	uint32_t dev_id;
	uint32_t timestamp;
	uint16_t packet_number;
	param_t param;
}request_t;
#pragma pack(pop)

class Device
{
		private:
			uint16_t number_sent;											//Sequential number of sent packet, 
		    uint16_t number_recieved;										//Sequential number of recieved packet,
			uint32_t dev_ip;
			
		public:
			Device(uint32_t dev_IP);
			Device();
			request_t recieved_data;
			uint16_t get_number_sent() const;
			uint16_t get_number_recieved() const;
			uint16_t get_number_dev_ip() const;
			std::atomic<bool> recieved;//Set false at the moment of sending reading request
									   //Set true when the data is recieved ot timeout is over
			void number_sent_up();
			void number_recieved_up();
			
};


class ControlConstants
{
	private:
#ifdef _WIN32
	    WSADATA wsa;
#endif
		std::map<uint32_t, std::shared_ptr<Device>> map_of_device;				//There are addresses, sequential number of sent packet, 
																				//sequential number of recieved packet kept in the Device
		std::mutex mtx;
		
		uint16_t recommended_parametr[COUNT_OF_COMMAND] = {0x0001, 0x4001,		//First two addresses for reading, left are for writing
														0x0006, 0x0008};
		int socket_;
		std::atomic<bool> is_it_run;											//Variable to control thread
		std::thread reciever_thread;											//Thread reciever
		ControlConstants();														//Private constructor
		void thread_recieve_data();												//Endless func for reciever
		uint8_t send_request(request_t* data);									//Func is used to form and send udp packets


		uint32_t data_32_swap(uint32_t data);//Swaps 32 bits data
		uint16_t data_16_swap(uint16_t data);//Swaps 16 bits data

		/*
			@brief - func  enables or reenables activity of class(thread and socket)
			@param - None
		*/
		void enable_socket_and_thread();
	public:
		~ControlConstants();

		/*
			@brief - Fubc returns a pointer to an objects, pattern Singleton is used
			@param - None
		*/
		static ControlConstants* get_ControlConstants();

		/*
			@brief - Func sends request_t over udp
			@param - data pointer which was formed and must be transmitted,  TYPE_OF_REQUEST defines read request or write will be sent 
		*/
		uint8_t do_request(uint8_t TYPE_OF_REQUEST, request_t* data);


		/*
			@brief - Func creates request for reading
			@param - data pointer to request which will be formed, dev_id ip address of device, TYPE_OF_COMMAND type of command check in CC.h class
		*/
		void make_read_request (request_t* data, uint32_t dev_id, uint8_t TYPE_OF_COMMAND);

		/*
			@brief - Func creates request for writing
			@param - data pointer to request which will be formed, dev_id ip address of device, TYPE_OF_COMMAND type of command check in CC.h class, data_to_write this data must be written, size of data
		*/
		void make_write_request (request_t* data,  uint32_t dev_id, uint8_t TYPE_OF_COMMAND, const uint8_t* data_to_write,  int16_t size);

		/*
			@brief - Func checks if class active or not
			@param - None
		*/
		bool is_working() const;


		/*
			@brief - Func closes socket and ends thread
			@param - None
		*/
		void stop_socket_and_thread();

		
		
};		



#endif
