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
//Возможные порты для прослушивания и отправки, для теста на одном компьютере порты будут разными
#define PORTLISTEN             32766
#define PORTSEND               8888
//
//Есть массив возможных адресов комманд, по индексу из макроса будем обращаться к команде равен адресу команды
#define TEST_ADDR_COMMAND      0
#define TEST_ADDR_COMMAND_2    1
#define TEST_ADDR_COMMAND_3    2
//
#define LENGTH_DATA_PART       8
#define LENGTH_OF_BUFFER       1024
#define TEST_IP                0xc0a8c915  //192.168.201.21
#define BROADCAST_IP           "127.0.0.1" //"255.255.255.255"

//Типы запросов
#define WRITE_REQ               1
#define READ_REQ                2
//

#define SUCCESS                 0
#define FAILURE                 1



#pragma pack(push, 1)//Отключаем выравнивание структур
typedef struct  
{
	uint16_t address;
	uint8_t data[LENGTH_DATA_PART];//При запросе на чтение обязательно должно быть заполнено нулями
}param_t;


typedef struct  
{
	uint16_t header;
	uint32_t dev_id;
	time_t timestamp;
	uint16_t packet_number;
	param_t param;
}request_t;
#pragma pack(pop)

class Device
{
		private:
			uint16_t number_sent;//Номер отправленного пакета
		    uint16_t number_recieved;//Номер последнего полученного пакета
			uint32_t dev_ip;
			
		public:
			Device(uint32_t dev_IP);
			uint16_t get_number_sent() const;
			uint16_t get_number_recieved() const;
			uint16_t get_number_dev_ip() const;
			std::atomic<bool> recieved;//Выставляем false в момент, когда оптарвляем запрос на чтение
									   //Выставляем true в момент, когда получим данные, либо когда выйдет timeout
			void number_sent_up();
			void number_recieved_up();
			
};


class ControlConstants
{
	private:
#ifdef _WIN32
	    WSADATA wsa;
#endif
		std::map<uint32_t, std::unique_ptr<Device>> map_of_device;
		std::mutex mtx;
		uint16_t recommended_parametr[3] = {0x0001, 0x0004, 0x0006};
		SOCKET socket_;
		std::atomic<bool> is_it_run;//Переменная для отключения потока
		std::thread reciever_thread;//Поток получатель 
		ControlConstants();//Конструктор делаем приватным чтобы объект был только одним Singleton
		void thread_recieve_data();//Функция которая будет вечно крутиться в потоке и принимать данные
		uint8_t send_request(const request_t& data);
		
	public:
		~ControlConstants();
		static ControlConstants& get_ControlConstants();
		uint8_t do_request(uint8_t TYPE_OF_REQUEST, request_t* data);
		
		//uint8_t read_request(request_t& data);
		
		
};		



#endif
