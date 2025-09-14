#ifndef  CONTROLCONSTANTSOVERUDP
#define  CONTROLCONSTANTSOVERUDP
//��� ���ᯫ���ଥ����� ������塞 �஢�ન, �� �ᯮ�짮����� Qt ����� �⮣� ��������
//⠪ ��� ���� QNetwork
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
//�������� ����� ��� ����訢���� � ��ࠢ��, ��� ��� �� ����� �������� ����� ���� ࠧ�묨
#define PORTLISTEN             32766
#define PORTSEND               8888
//
//���� ���ᨢ ��������� ���ᮢ �������, �� ������� �� ����� �㤥� �������� � ������� ࠢ�� ����� �������
#define COUNT_OF_COMMAND       4//����饥 ������⢮ �������
#define TEST_ADDR_COMMAND      0//��� �⥭�� ����
#define TEST_ADDR_COMMAND_1    1//��� �⥭�� ����
#define TEST_ADDR_COMMAND_2    2//��� ����� ����
#define TEST_ADDR_COMMAND_3    3//��� ����� ����
//
#define LENGTH_DATA_PART       8
#define LENGTH_OF_BUFFER       1024

#define BROADCAST_IP           "127.0.0.1" //"255.255.255.255"

//���� ����ᮢ
#define WRITE_REQ               1
#define READ_REQ                2
//

#define SUCCESS                 0
#define FAILURE                 1



#pragma pack(push, 1)//�⪫�砥� ��ࠢ������� �������, �ਢ�筥� �뫮 ������ __attached__ �� �� ��� ��������� �� ���ᯫ���ଥ���
typedef struct  
{
	uint16_t address;
	uint8_t data[LENGTH_DATA_PART];//�� ����� �� �⥭�� ��易⥫쭮 ������ ���� ��������� ��ﬨ
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
			uint16_t number_sent;//����� ��ࠢ������� �����
		    uint16_t number_recieved;//����� ��᫥����� ����祭���� �����
			uint32_t dev_ip;
			
		public:
			Device(uint32_t dev_IP);
			Device();
			request_t recieved_data;
			uint16_t get_number_sent() const;
			uint16_t get_number_recieved() const;
			uint16_t get_number_dev_ip() const;
			std::atomic<bool> recieved;//���⠢�塞 false � ������, ����� ���ࢫ塞 ����� �� �⥭��
									   //���⠢�塞 true � ������, ����� ����稬 �����, ���� ����� �멤�� timeout
			void number_sent_up();
			void number_recieved_up();
			
};


class ControlConstants
{
	private:
#ifdef _WIN32
	    WSADATA wsa;
#endif
		std::map<uint32_t, std::shared_ptr<Device>> map_of_device;//����� �࠭�� ����, ���浪��� ����� ��ࠢ������� �����, 
		//���浪��� ����� ����祭���� ����� 
		std::mutex mtx;
		//��� ����� ���� ��� �⥭��, ��� ����� ��� �����
		uint16_t recommended_parametr[COUNT_OF_COMMAND] = {0x0001, 0x4001, 0x0006, 0x0008};
		SOCKET socket_;
		std::atomic<bool> is_it_run;//��६����� ��� �⪫�祭�� ��⮪�
		std::thread reciever_thread;//��⮪ �����⥫� 
		ControlConstants();//��������� ������ �ਢ��� �⮡� ��ꥪ� �� ⮫쪮 ����� Singleton
		void thread_recieve_data();//�㭪�� ����� �㤥� ��筮 �������� � ��⮪� � �ਭ����� �����
		uint8_t send_request(request_t* data);//�㭪�� ��� ��ࠢ�� udp ᮪��
		uint32_t data_32_swap(uint32_t data);//��ॢ���稢��� 32 ���� �����
		uint16_t data_16_swap(uint16_t data);//��ॢ���稢��� 16 ���� �����
	public:
		~ControlConstants();
		static ControlConstants& get_ControlConstants();//������ �㭪�� �ᯮ��㥬 ��� ����祭�� �����⢥����� ������ �����
		uint8_t do_request(uint8_t TYPE_OF_REQUEST, request_t* data);//������ �㭪�� �ᯮ������ ���짮��⥫�� ������⥪� ��� ⮣�, �⮡� ��ࠢ��� ����室��� �����
																     //��� �⮣� �ᯮ������� ������ WRITE_REQ � READ_REQ � TYPE_OF_REQUEST
		void make_read_request (request_t* data, uint32_t dev_id, uint8_t TYPE_OF_COMMAND);//� ������� ������ �㭪樨 �� ������ ����� �� �⥭��
		void make_write_request (request_t* data,  uint32_t dev_id, uint8_t TYPE_OF_COMMAND, const uint8_t* data_to_write,  uint8_t size);//� ������� �⮩ ������� ��
																																		  //������ ����� �� ������
		//uint8_t read_request(request_t& data);
		
		
};		



#endif
