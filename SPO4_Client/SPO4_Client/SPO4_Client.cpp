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
	SOCKET server_sock;
	sockaddr_in dest_addr;
	WSADATA wsaData;
	if (WSAStartup(0x202,&wsaData))
		throw "Starting fail!\n";
	else
		cout << "Started!\n";

	char *cIP = new char[16];

	cout << "Enter IP adress\n";
	cin >> cIP;

	
	server_sock = socket(AF_INET,SOCK_STREAM,0);
	if (server_sock == INVALID_SOCKET)
		throw "Socket creation error!\n";
	else
		cout << "Socket created!\n";
	
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(5555);
	dest_addr.sin_addr.s_addr = inet_addr(cIP);

	if(connect(server_sock,(sockaddr*)&dest_addr,sizeof(dest_addr)))
		throw "Connection fail!\n";
	else
		cout << "Connected!\n";

	return server_sock;
}

void GetData(ofstream *file, SOCKET server_sock, long int size, char *mes)
{
	char *buffer;
	if (size < MAX_SIZE)
		buffer = new char[size];
	else
		buffer = new char[MAX_SIZE];

	int readed = 0, remain = size;
	int msize;
	char c;

	bool abort = false;
	while (remain > 0)
	{
					
		if (_kbhit())
		{
			c = _getch();
			if (c == 'p')
			{
				if (SOCKET_ERROR == send(server_sock, &c, 1, MSG_OOB)) throw "Sending error!";
				abort = true;
			}
		}
		if (abort)
		{
			struct fd_set SOCKS;
			FD_ZERO(&SOCKS);

			struct timeval *time = new timeval;
			time->tv_usec = 200000;
			FD_SET(server_sock, &SOCKS);
			strcpy_s(mes, 255, "Aborted!");
			break;
		}
		if (SOCKET_ERROR == (msize = recv(server_sock, buffer, (remain>MAX_SIZE)?MAX_SIZE:remain, 0))) throw "Receiving error!";
		file->write (buffer, msize);
		readed += msize;
		remain -= msize;
		printf("\rSended %ld/%ld\t\t\t", readed, size);	
	}
}

int GettingFile(SOCKET server_sock, char *mes)
{
	char* filename = NULL;
	char* cPath = NULL;
	long int size;

	if (strstr(mes, "getfile"))
	{
		size = strlen(mes)-1;

		if(!filename)
			filename = new char[256];
		strcpy_s(filename, 256, mes);

		if (SOCKET_ERROR == recv(server_sock, mes, 256,0)) throw "Receiving error!";
		size = atoi(mes);
		if (size >= 0)
			printf ("Size of file: %ld bytes. Want to download it? (yes/no) ", size);
		else
		{
			cout << "File not exist!\n";
			return -1;
		}
		cin.getline(mes, 256);
		if (SOCKET_ERROR == send(server_sock, mes, 256, 0)) throw "Sending error!";
		if (strcmp(mes, "yes"))
			return -1;
		if(!cPath)
			cPath = new char[MAX_PATH];

		cout << "Enter file path and name for save: ";
		cin >> cPath;

		ofstream file(cPath, ios::binary);
		if (!file.is_open()) throw "Govno path!";

		if (SOCKET_ERROR == send(server_sock, "get", 256, 0)) throw "Sending error!";

		GetData(&file, server_sock, size, mes);
		
		cout << '\n';
		file.close();
	}
}

void main()
{
	try
	{
		SOCKET server_sock;
		char *mes = new char[256];

		server_sock = OnInit();


		while (1)
		{
			fflush(stdin);
			cin.getline(mes, 256);
			if (SOCKET_ERROR == send(server_sock, mes, 256, 0)) throw "Sending error!";

			if (!strcmp(mes, "exit")) break;
			if (!strcmp(mes, "die")) break;

			GettingFile(server_sock, mes);

			if (SOCKET_ERROR == recv(server_sock, mes, 256, 0)) throw "Receiving error!";
			cout << mes << endl;
		}
		closesocket(server_sock);
	}
 	catch (const char* message)
	{
		cout << "ERROR: " << message;
		WSACleanup();
		cout << "Cleanup OK\n";
		_getch();
	}
}
