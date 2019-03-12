#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>


#define prefix "http://"
#define dns_server_name "www.iu3.bmstu.ru"
#define local_path "/WebApi/time"
#define http_port 80

// Глобальная переменная для сокета
int mysocket;
struct sockaddr_in server_socket_info;
struct hostent * address_info;

/* Функция создает соединение с сервером */
int connect_to_server() {
	// используем Address Family для IPv4 и протокол TCP
	mysocket = socket(AF_INET, SOCK_STREAM, 0);
	if (mysocket < 0) {
		printf("Error creating socket!\n");
		return -1;
	}
	// Получение структуры hostent
	address_info = gethostbyname(dns_server_name);
	if (address_info == NULL) {
		printf("Error getting IP address information!\n");
		close(mysocket);
		return -1;
	}
	// Подготовка структуры sockaddr_in (заполняем нулями)
	memset(&server_socket_info, 0, sizeof(server_socket_info));
	// Семейство адресов - IPv4
	server_socket_info.sin_family = AF_INET;
	// Копирование первого адреса из address_info
	memcpy(&server_socket_info.sin_addr.s_addr, address_info->h_addr, address_info->h_length);
	// Задаем номер порта сервера в формате big-endian
	server_socket_info.sin_port = htons(http_port);
	// Начало соединения
	if (connect(mysocket, (struct sockaddr *)&server_socket_info,sizeof(server_socket_info)) < 0) {
		printf("Error connecting to server \"%s\"!\n", dns_server_name);
		close(mysocket);
		return -1;
	}
	else {
		printf("Successfully connected to server \"%s\"!\n", dns_server_name);
		return 0;
	}

}


int send_request_to_server() {
	int request_length = 0;
	int n = 0;
	//char * request = "GET / HTTP/1.0\r\nHost: ";
	//strcat(request, dns_server_name);
	//strcat(request, "\r\n\r\n");
	char * request = "GET / HTTP/1.0\r\nHost: www.iu3.bmstu.ru\r\n\r\n";
	char server_response[256];
	// Определяем длину запроса
	request_length = strlen(request);
	// Посылаем запрос блоками
	while (request_length > 0) {
		n = write(mysocket, &request[n], request_length);
		if (n < 0) {
			printf("Error sending request!\n");
			close(mysocket);
			return -1;
		}
		request_length -= n;
	}
	// В бесконечном цикле слушаем ответ сервера
	for(;;) {
		n = read(mysocket, server_response, 255);
		// Пока есть данные - считываем их
		if (n > 0) {
			server_response[n] = 0;
			printf("%s\n",server_response);
		}
		else if (n < 0) {
			printf("Error sending request!\n");
			close(mysocket);
			return -1;
		}
		else break;
	}
	close(mysocket);
	return 0;
}


int main(int argc, char * argv[])
{
	int res;
	res = connect_to_server();
	if (res == 0) {
		send_request_to_server();
	}
	return 0;
}



