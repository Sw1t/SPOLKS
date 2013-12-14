#include "stdafx.h"
#include <winsock.h>
#include <iostream>
#include <fstream>
#include <conio.h>

using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#define MAX_SIZE 20480


SOCKET OnInit()
{
	WSADATA wsaData;
	SOCKET server_sock;

	if (WSAStartup(0x202,&wsaData))
		throw "Starting fail!\n";
	else
		cout << "Started!\n";
	
	server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sock == INVALID_SOCKET)
		throw "Socket creation error!\n";
	else
		cout << "Socket created!\n";

	return server_sock;
}

void Connection(SOCKET server_sock )
{
	sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(5555);
	serv_addr.sin_addr.s_addr = 0;

	if (bind(server_sock, (sockaddr*)&serv_addr, sizeof(serv_addr)))
		throw "Binding error!\n";
	else
		cout << "Binding success!\n";

	if(listen(server_sock, 10) == 0)
		cout << "Listen succes!\n";
}

int Sending(ifstream *file, int size, SOCKET client_sock)
{
	char *buffer, *mes;
	if (size < MAX_SIZE)
		buffer = new char[size];
	else
		buffer = new char[MAX_SIZE];
	mes = new char[256];

	int sended = 0, remain = size;
	int msize;
	char c;

	struct fd_set SOCKS;
	FD_ZERO(&SOCKS);

	struct timeval *time = new timeval;
	time->tv_sec = 0;
	time->tv_usec = 100;
	while (remain > 0)
	{
		FD_SET(client_sock, &SOCKS);
		if(select(0,NULL,NULL,&SOCKS,time) > 0)
		{
			if (SOCKET_ERROR == recv(client_sock, &c, 1, MSG_OOB)) throw "Receiving error!";
			strcpy_s(mes, 255,"Download aborted!\n");
			remain = -1;
			break;
		}

		memset(buffer, 0, sizeof(buffer));
		file->read(buffer,(remain>MAX_SIZE)?MAX_SIZE:remain);
		if (SOCKET_ERROR == (msize = send(client_sock, buffer, (remain>MAX_SIZE)?MAX_SIZE:remain, 0))) throw "Sending error!";
							
		sended += msize;
		remain -= msize;
	}
	printf("\rSended %ld bytes\t\t\t\n", sended);
	return remain;
}

bool SendFile( SOCKET client_sock, char *mes)
{
	
	ifstream file(mes+8, ios::binary);
	int remain = 0;

	if (file.is_open())
	{
		file.seekg(0,ios::end);
		long size = file.tellg();
		file.seekg(0,ios::beg);

		_itoa_s((int)size, mes, 256, 10);

		if (SOCKET_ERROR == send(client_sock, mes, strlen(mes)+1, 0)) throw "Sending error!";
		if (SOCKET_ERROR == recv(client_sock, mes, 256, 0)) throw "Receiving error!";
		if (strcmp(mes, "yes"))
		{
			file.close();
			return false;
		}

		remain = Sending(&file, size, client_sock);
						
		if (SOCKET_ERROR == recv(client_sock, mes, 256, 0)) throw "Receiving error!";
		if (strcmp(mes, "get"))
			return false;
				
		
		if (!remain)
			strcpy_s(mes, 256,"Finished!\n");
		else
			strcpy_s(mes, 256,"Download aborted!\n");
		file.close();
	}
	else
	{
		strcpy_s(mes, 256,"-1");
		return false;
	}
	return true;
}

char *Functional( SOCKET client_sock )
{
	char *mes = new char[256];
	while(1)
	{
		if (SOCKET_ERROR == recv(client_sock, mes, 256, 0)) throw "Receiving error!";

		if (!strcmp(mes, "exit"))
			break;
		if (!strcmp(mes,"die"))
			return mes;

		if (!strcmp(mes,"hello")) strcpy_s(mes, 256, "Hello, my friend!\n");	
		else
		if (strstr(mes, "getfile"))
		{
			if(SendFile(client_sock, mes) == false)
			{
				continue;
			}
		}
		else
			strcpy_s(mes,256,"Unrecognized command!\n");

		cout << "ITS ALL!!\n";
		if (SOCKET_ERROR == send(client_sock, mes, 256, 0)) throw "Sending error!";
	}
	return mes;
}

void main()
{
	try
	{
		char *mes = new char[256];
		SOCKET server_sock;
		SOCKET client_sock;
		

		server_sock = OnInit();
		Connection( server_sock );

		while (1)
		{
			client_sock = accept(server_sock, 0, 0);
			cout << "Client connected!\n";

			mes = Functional(client_sock);
			if(!strcmp(mes, "die"))
				return;

			cout << "Connection complete!\n";
		}

		closesocket(server_sock);
		closesocket(client_sock);
	}
	catch (const char* message)
	{
		cout << "ERROR: " << message;
		WSACleanup();
		cout << "\nCleanup OK\n";
	}
	_getch();
	return;
}

