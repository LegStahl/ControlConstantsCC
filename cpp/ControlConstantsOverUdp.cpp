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
	reciever_thread.detach();
	socket_ = socket(AF_INET, SOCK_DGRAM, 0);
	
	if (socket_ < 0 ) {
		perror("socket failed");
		is_it_run = false;
	}
}	

ControlConstants::~ControlConstants()
{
	is_it_run = false;
	
	
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
		is_it_run = false;
	}
	char buffer[LENGTH_OF_BUFFER];
	socklen_t srcLen = sizeof(addr);
	while (is_it_run) 
	{
		
		int bytes = recvfrom(socket_, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&addr, &srcLen);
		std::cout << "Data was" << std::endl;
		if (bytes ==  sizeof(request_t))
		{
			
			request_t temp;
			std::memcpy(&temp, buffer, sizeof(request_t));
			if(map_of_device.count(temp.dev_id) == 0)//Чтобы сравнивать адреса за пределами данного программного модуля их необходимо будет переворачивать обратно
			{
				std::cout << "Address from net : " << temp.dev_id << std::hex << "\n";
				map_of_device[temp.dev_id] = std::make_shared<Device>(temp.dev_id);
				map_of_device[temp.dev_id]->recieved_data = temp;
			}
			std::cout << "Address from net : " << temp.dev_id << std::hex << "\n";
			std::shared_ptr<Device> temp_ptr = map_of_device[temp.dev_id];
			temp_ptr->recieved_data = temp;
			temp_ptr->recieved = true;
			std::cout << "Data was recieved!" << std::endl;
		}
	}

}

uint8_t ControlConstants::send_request(request_t* data)
{
	sockaddr_in dest{};
    dest.sin_family = AF_INET;
    dest.sin_port = htons(PORTSEND);
    dest.sin_addr.s_addr = inet_addr(BROADCAST_IP);
	char buffer[LENGTH_OF_BUFFER];
	std::memcpy(buffer, data, sizeof(*data));
	std::cout << "SIZE OF ALL DATA " << sizeof(*data) << std::endl;
	std::cout << "SIZE OF PARAM " << sizeof(data->param) << std::endl;
	std::cout << "SIZE OF time " << sizeof(data->timestamp) << std::endl;
	for (size_t i = 0; i < sizeof(*data); ++i) {
        std::cout << std::hex << std::uppercase 
                  << std::setw(2) << std::setfill('0')
                  << static_cast<int>(buffer[i]) << " ";
    }
    sendto(socket_, buffer, sizeof(*data), 0, (struct sockaddr*)&dest, sizeof(dest));
	return 0;
}




uint8_t ControlConstants::do_request(uint8_t TYPE_OF_REQUEST, request_t* data)
{
	std::lock_guard<std::mutex> lock(mtx);//Закрываем mutex на время действия функции
	if(map_of_device.count(data->dev_id) == 0)
	{	
		map_of_device[data->dev_id] = std::make_shared<Device>(data->dev_id);
	}
	
	uint8_t STATUS = FAILURE;
	if(TYPE_OF_REQUEST == READ_REQ && (data->header & 0x0003) == 0)//Проверка запроса на чтение, не на запись и не ответ 
	{
		std::shared_ptr<Device> temp_ptr = map_of_device[data->dev_id];
		std::fill(data->param.data, data->param.data + LENGTH_DATA_PART, 0);//Для чтения заполняем нулями по протоколу
		data->timestamp = (uint32_t)time(0);
		data->packet_number = temp_ptr->get_number_sent();
		temp_ptr->recieved = false;//
		temp_ptr->number_sent_up();
		send_request(data);
		time_t seconds = time(0);
	
		while(((time(0) - seconds) <= 1) && !(temp_ptr->recieved))
		{
		}
		if(temp_ptr->recieved)
		{
			std::cout << "Success in recieveng data!" << std::endl;
			*data = temp_ptr->recieved_data;
			STATUS = SUCCESS;
		}
		
	}
	if(TYPE_OF_REQUEST == WRITE_REQ && (data->header & 0x0002) != 0)//Проверка запрос на запись, пока предположу, что запрос на запись это 0x0002
	{
		
	}
	return STATUS;
}

void ControlConstants::make_read_request(request_t* data, uint32_t dev_id, uint8_t TYPE_OF_COMMAND)
{
	data->header = data_16_swap(0xcc30);//Header для запроса на чтение
	data->dev_id = data_32_swap(dev_id);//Устанавливаем dev_id
	if(TYPE_OF_COMMAND < COUNT_OF_COMMAND)
		data->param.address = data_16_swap(recommended_parametr[TYPE_OF_COMMAND]);
	//data в param не заполняем для чтения, packet_number тоже не заполняем и временную метку поставим только во время отправки
}

void ControlConstants::make_write_request(request_t* data, uint32_t dev_id, uint8_t TYPE_OF_COMMAND, const uint8_t* data_to_write, uint8_t size)
{
	data->header = data_16_swap(0xcc32);//Header для запроса на чтение
	data->dev_id = data_32_swap(dev_id);//Устанавливаем dev_id
	if(TYPE_OF_COMMAND < COUNT_OF_COMMAND)
		data->param.address = data_16_swap(recommended_parametr[TYPE_OF_COMMAND]);
	for(uint8_t i = size - 1, j = 0; i >= 0; i--, j++)
	{
		data->param.data[i] = data_to_write[j];//Записываем данные задом наперед
	}
}

uint32_t  ControlConstants::data_32_swap(uint32_t data)
 {
    return ((data & 0xFF000000) >> 24) |
           ((data & 0x00FF0000) >> 8)  |
           ((data & 0x0000FF00) << 8)  |
           ((data & 0x000000FF) << 24);
}

uint16_t  ControlConstants::data_16_swap(uint16_t data)
 {
    return (data >> 8) | (data << 8);         
}

Device::Device()
{
	recieved = true;
	number_sent = 0;
	number_recieved = 0;
}

Device::Device(uint32_t dev_IP) : dev_ip(dev_IP)
{
	recieved = true;
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