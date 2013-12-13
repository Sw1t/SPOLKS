
#include "stdafx.h"
#include <winsock.h>
#include <iostream>
using namespace std;


#pragma comment(lib, "Ws2_32.lib")

int main()
{
	try
	{

		WSADATA wsaData;
		if (WSAStartup(0x202,&wsaData))
			throw "Starting fail!\n";
		else
			cout << "Started!\n";

		SOCKET server_sock;
		server_sock = socket(AF_INET,SOCK_STREAM,0);
		if (server_sock == INVALID_SOCKET)
			throw "Socket creation error!\n";
		else
			cout << "Socket created!\n";

		sockaddr_in serv_addr;
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(5555);
		serv_addr.sin_addr.s_addr = 0;

		if (bind(server_sock,(sockaddr*)&serv_addr,sizeof(serv_addr)))
			throw "Binding error!\n";
		else
			cout << "Binding success!\n";

		if(listen(server_sock, 10) == 0)
			cout << "Listen succes!\n";

		sockaddr_in client_addr;
		SOCKET client_sock;
		int size_client_addr = sizeof(client_addr);

		char *mes = new char[256];
		client_sock = accept(server_sock,(sockaddr*)&client_addr,&size_client_addr);
		while (1)
		{
			if (SOCKET_ERROR == recv(client_sock, mes, 256, 0)) throw "Receiving error!";
			if (!strcmp(mes,"exit")) break;
			if (!strcmp(mes,"hello")) _snprintf_s(mes, 256, 256, "Hello, my friend!\n");
			if (!strcmp(mes,"me")) _snprintf_s(mes, 256, 256, "IP %s port %d\n\r", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
			if (SOCKET_ERROR == send(client_sock, mes, 256, 0)) throw "Sending error!";
		}
		cout << "Accept complete!\n";
	}
	catch (const char* message)
	{
        cout << "ERROR: " << message;
		WSACleanup();
		cout << "Cleanup OK\n";
	}

	cin.get();
	return 0;
}

