#include "stdafx.h"
#include <winsock.h>
#include <iostream>
#include <fstream>
#include <stdio.h>

using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#define MAX_SIZE 2048


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
		server_sock = socket(AF_INET, SOCK_STREAM, 0);
		if (server_sock == INVALID_SOCKET)
			throw "Socket creation error!\n";
		else
			cout << "Socket created!\n";

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

		SOCKET client_sock;

		while (1)
		{
			char *mes = new char[256];
			client_sock = accept(server_sock, 0, 0);
			cout << "Client connected!\n";
			while (1)
			{
				if (SOCKET_ERROR == recv(client_sock, mes, 256, 0)) throw "Receiving error!";

				if (!strcmp(mes, "exit"))
					break;
				else if (!strcmp(mes,"die"))
					return 0;

				if (!strcmp(mes,"hello")) strcpy_s(mes, 256, "Hello, my friend!\n");

				else if (strstr(mes, "getfile"))
				{
					ifstream file(mes+8, ios::binary);

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
							continue;
						}

						char *buffer;
						if (size < MAX_SIZE)
							buffer = new char[size];
						else
							buffer = new char[MAX_SIZE];

						int sended = 0, remain = size;
						int msize;
						while (remain > 0)
						{
							memset(buffer,0,sizeof(buffer));
							file.read(buffer,(remain>MAX_SIZE)?MAX_SIZE:remain);
							if (SOCKET_ERROR == (msize = send(client_sock, buffer, (remain>MAX_SIZE)?MAX_SIZE:remain, 0))) throw "Sending error!";
							sended += msize;
							remain -= msize;
						}
						file.close();
						if (!remain)
							strcpy_s(mes, 256,"Finished!\n");
						else
							strcpy_s(mes, 256,"Download aborted!\n");
					}
					else
						strcpy_s(mes, 256,"-1");
				}
				else
					strcpy_s(mes,256,"Unrecognized command!\n");

				cout << "ITS ALL!!\n";
				if (SOCKET_ERROR == send(client_sock, mes, 256, 0)) throw "Sending error!";
			}
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

	return 0;
}

