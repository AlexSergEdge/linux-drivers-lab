#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <getopt.h>
#include <locale.h>
#include <stdlib.h>


#define prefix "http://"
#define dns_server_name "www.iu3.bmstu.ru"
#define local_path "/WebApi/time"
#define http_port 8090

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
		printf("Successfully connected to server \"%s\"!\n\n", dns_server_name);
		return 0;
	}

}


int send_request_to_server(char * request) {
	int request_length = 0;
	int n = 0;
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
	// Флаг для проверки того, что запрос успешен
	char response_checked = NULL;
	for(;;) {
		n = read(mysocket, server_response, 255);
		// Пока есть данные - считываем их	
		if (n > 0) {
			if (strstr(server_response, "HTTP/1.1 200 OK") == NULL && !response_checked) {
				printf("Server responce was not OK\n");
				return 0;
			}
			response_checked = '1';
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

#define DEFAULT_REQUEST_TYPE "get"
#define DEFAULT_TIME_TYPE "local"
#define DEFAULT_TIME_FORMAT "internet"

void print_usage() {
	printf("Usage:\n ./rest [-r get/post] [-t local/utc] [-f internet/unix]\nFlags are not mandatory\n");
	printf("If flags (some or all) are not specified, default values are used:\n ./rest -r get -t local -f internet\n");
}

int main(int argc, char * argv[])
{
	setlocale(LC_ALL, "ru_RU.UTF8");

	char request_type[256] = DEFAULT_REQUEST_TYPE; 
	char time_type[256] = DEFAULT_TIME_TYPE;
	char time_format[256] = DEFAULT_TIME_FORMAT;

	char * request = (char*)malloc(512);
	int opt;
	while((opt = getopt(argc, argv, ":r:t:f:")) != -1) {
		switch(opt) {
			case 'r':
				if (strcmp(optarg, "get") == 0 || strcmp(optarg, "post") == 0) {
					// optarg contains get/post
					printf("%s\n", optarg);
					strcpy(request_type, optarg);
					break;
				}
				else {
					printf("Unknown request type: %s\n", optarg);
					print_usage();
					return 0;
				}
			case 't':
				if (strcmp(optarg, "utc") == 0 || strcmp(optarg, "local") == 0) {
					// optarg contains utc/local
					printf("%s\n", optarg);
					strcpy(time_type, optarg);
					break;
				}
				else {
					printf("Unknown time type: %s\n", optarg);
					print_usage();
					return 0;
				}
			case 'f':
				if (strcmp(optarg, "unix") == 0 || strcmp(optarg, "internet") == 0) {
					// optarg contains unix/internet
					printf("%s\n", optarg);
					strcpy(time_format, optarg);
					break;
				}
				else {
					printf("Unknown time format: %s\n", optarg);
					print_usage();
					return 0;
				}
			case '?':
				printf("Unknown option: %c !\n", optopt);
				print_usage();
				return 0;
			case ':':
				printf("Argument is missing for: %c !\n", optopt);
				print_usage();
				return 0;
		}
	}
	// Now we can form request
	if (strcmp(request_type, "get") == 0) {
		sprintf(request, "GET /WebApi/time?type=%s&format=%s HTTP/1.0\n\n", time_type, time_format);
	}
	else {
		sprintf(request, "POST /WebApi/time HTTP/1.0\nContent-Type:application/x-www-form-urlencoded\nContent-Length:20\n\ntype=%s&format=%s\n", time_type, time_format);
	}
	printf("Request:\n%s\n", request); 
	int res;
	res = connect_to_server();
	if (res == 0) {
		send_request_to_server(request);
	}
	free(request);
	return 0;
}
