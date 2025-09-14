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
#define COUNT_OF_COMMAND       4//Текущее количество комманд
#define TEST_ADDR_COMMAND      0//Для чтения адрес
#define TEST_ADDR_COMMAND_1    1//Для чтения адрес
#define TEST_ADDR_COMMAND_2    2//Для записи адрес
#define TEST_ADDR_COMMAND_3    3//Для записи адрес
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



#pragma pack(push, 1)//Отключаем выравнивание структур, привычнее было делать __attached__ но он как оказалось не кроссплатформенный
typedef struct  
{
	uint16_t address;
	uint8_t data[LENGTH_DATA_PART];//При запросе на чтение обязательно должно быть заполнено нулями
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
			uint16_t number_sent;//Номер отправленного пакета
		    uint16_t number_recieved;//Номер последнего полученного пакета
			uint32_t dev_ip;
			
		public:
			Device(uint32_t dev_IP);
			Device();
			request_t recieved_data;
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
		std::map<uint32_t, std::shared_ptr<Device>> map_of_device;//Здесь храним адрес, порядковый номер отправленного пакета, 
		//порядковый номер полученного пакета 
		std::mutex mtx;
		//Два первых адреса для чтения, два вторых для записи
		uint16_t recommended_parametr[COUNT_OF_COMMAND] = {0x0001, 0x4001, 0x0006, 0x0008};
		SOCKET socket_;
		std::atomic<bool> is_it_run;//Переменная для отключения потока
		std::thread reciever_thread;//Поток получатель 
		ControlConstants();//Конструктор делаем приватным чтобы объект был только одним Singleton
		void thread_recieve_data();//Функция которая будет вечно крутиться в потоке и принимать данные
		uint8_t send_request(request_t* data);//Функция для отправки udp сокета
		uint32_t data_32_swap(uint32_t data);//Перевоарчивает 32 битные данные
		uint16_t data_16_swap(uint16_t data);//Перевоарчивает 16 битные данные
	public:
		~ControlConstants();
		static ControlConstants& get_ControlConstants();//Данную функцию используем для получения единственного экземляра класса
		uint8_t do_request(uint8_t TYPE_OF_REQUEST, request_t* data);//Данная функция используется пользователем библиотеки для того, чтобы отправить необходимый запрос
																     //для этого используеются макросы WRITE_REQ и READ_REQ в TYPE_OF_REQUEST
		void make_read_request (request_t* data, uint32_t dev_id, uint8_t TYPE_OF_COMMAND);//С помощью данной функции мы делаем запрос на чтение
		void make_write_request (request_t* data,  uint32_t dev_id, uint8_t TYPE_OF_COMMAND, const uint8_t* data_to_write,  uint8_t size);//С помощью этой команды мы
																																		  //делаем запрос на запись
		//uint8_t read_request(request_t& data);
		
		
};		



#endif
