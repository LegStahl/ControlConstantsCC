#include "ControlConstantsOverUdp.h"
ControlConstants::ControlConstants() : is_it_run(true)
{
	#ifdef _WIN32
	if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        std::cerr << "WSAStartup failed\n";
        //return 1;
    }
	#endif
	std::cout << "Constructor was called" << std::endl;
	reciever_thread = std::thread(&ControlConstants::thread_recieve_data, this);
	socket_ = socket(AF_INET, SOCK_DGRAM, 0);
	
	if (socket_ < 0 ) {
		perror("socket failed");	
	}
}	

ControlConstants::~ControlConstants()
{
	is_it_run = false;
	if (reciever_thread.joinable())//Поток не detach 
            reciever_thread.join();
	
	CLOSESOCK(socket_);
	#ifdef _WIN32
	WSACleanup();
	#endif
}

ControlConstants& ControlConstants::get_ControlConstants()
{
	  static ControlConstants object;//Статические переменные инициализируются только один раз, значит вызов конструктора будет один
      return object;
}

void ControlConstants::thread_recieve_data()
{
	std::cout << "In recieve data" << std::endl;
	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(PORTLISTEN);

	if (bind(socket_, (struct sockaddr*)&addr, sizeof(addr)) < 0) 
	{
		perror("bind");
		throw std::runtime_error("Не удалось привязать сокет");
	}
	char buffer[LENGTH_OF_BUFFER];
	socklen_t srcLen = sizeof(addr);
	while (is_it_run) 
	{
		
		int bytes = recvfrom(socket_, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&addr, &srcLen);
							 
		if (bytes > 0)
		{
			buffer[bytes] = '\0';
			std::cout << "Получено: " << buffer << std::endl;
					 
		}
	}

}

uint8_t ControlConstants::send_request(const request_t& data)
{
	sockaddr_in dest{};
    dest.sin_family = AF_INET;
    dest.sin_port = htons(PORTSEND);
    dest.sin_addr.s_addr = inet_addr(BROADCAST_IP);
	char buffer[LENGTH_OF_BUFFER];
	std::memcpy(buffer, &data, sizeof(data));
    sendto(socket_, buffer, sizeof(data), 0, (struct sockaddr*)&dest, sizeof(dest));
	return 0;
}




uint8_t ControlConstants::do_request(uint8_t TYPE_OF_REQUEST, request_t* data)
{
	std::lock_guard<std::mutex> lock(mtx);//Закрываем mutex на время действия функции
	uint8_t STATUS = FAILURE;
	if(TYPE_OF_REQUEST == READ_REQ)
	{
		std::fill(data->param.data, data->param.data + LENGTH_DATA_PART, 0);//Для чтения заполняем нулями по протоколу
		data->timestamp = time(0);
		send_request(*data);
		time_t seconds = time(0);
		//Device *dev = map_of_device
		while(((time(0) - seconds) <= 1) )
		{
			
		}
		
	}
	if(TYPE_OF_REQUEST == WRITE_REQ)
	{
		
	}
	return STATUS;
}


Device::Device(uint32_t dev_IP) : dev_ip(dev_IP)
{
	number_sent = 0;
	number_recieved = 0;
}
uint16_t Device::get_number_sent() const
{
	return number_sent;
}
uint16_t Device::get_number_recieved() const
{
	return number_recieved;
}
uint16_t Device::get_number_dev_ip() const
{
	return dev_ip;
}

void Device::number_sent_up()
{
	number_sent++;
}
void Device::number_recieved_up()
{
	number_recieved++;
}