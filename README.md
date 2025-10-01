# CONTROL CONSTANTS OVER UDP 
## RUS VERSION DESCRIPTION
Программный модуль ControlConstantsOverUdp используется для общения с устройствами в сети посредством UDP пакетов.
В данном программном модуле важно составлять request_t только через специальные методы модуля make_read_request и make_write_request.
Отправка сформированного request_t осуществляется посредством метода do_request в качестве парметра выступают макрос, который определит
чтение или запись и сам сформированный request_t. Возвращает функция request_t статус выполнения функции, в параметре который мы передавали по указателю
request_t будет содерджаться данные из устройства при успешном выполнении.

Внутри класса существует поток постоянно работающий на вход. Функция thread_recieve_data. При деструкторе поток выключается в других случаях, ОС закроет 
поток сама. Чтобы получить объект, с помщью которго можно управлять модулем нужно вызвать функцию get_ControlConstants.
Модуль был проверен с помощью python скрипта имитирующем работу устройства. Адрес использовался локальный. Порты использовались разные тк компьютер один.

В build/bin лежат бинарники
Чтобы собрать проект необходимо создать внутри protocol build
Затем cmake -S . -B build проинициализировать проект
cmake --build build --config=Release или Debug чтобы собрать проект
### ENG VERSION DESCRIPTION
Program module ControlConstantsOverUdp is used for communication with devices in network via UDP datagramms.
The module consists two files .h where classes are described and .cpp where functions and methods are implemented.
#### How to use:
To send a request it is necessary to create and fill request_t through special methods "make_read_request" and "make_write_request"
Sending this request is possible through method "do_request" params are MACRO which defines READING or WRITING and request_t that was created and initialized.
"do_request" returns status of performing function, in request_t that was passed as param, will be new data in case of READING request 

There is a reciever thread inside class "thread_recieve_data". There are special methods for opening this thread and closing it "stop_socket_and_thread" and "enable_socket_and_thread".

"enable_socket_and_thread" is private method that are called when func "get_ControlConstants" called. 

In build/bin are executable code
Чтобы собрать проект необходимо создать внутри protocol build
To make project there is a need to make dir /build in /protocol
then cmake -S . -B build to init project
cmake --build build --config=Release or Debug to build project and to get executable
