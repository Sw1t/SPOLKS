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

		char *cIP = new char[16];

		cout << "Enter IP adress\n";
		cin >> cIP;

		sockaddr_in dest_addr;
		dest_addr.sin_family = AF_INET;
		dest_addr.sin_port = htons(5555);
		dest_addr.sin_addr.s_addr = inet_addr(cIP);

		if(connect(server_sock,(sockaddr*)&dest_addr,sizeof(dest_addr)))
			throw "Connection fail!\n";
		else
			cout <<"Connected!\n";

		char *buf = new char[256];
		int msize = 0;
		while (1)
		{
			cin.getline(buf,256);
			if (SOCKET_ERROR == send(server_sock,buf,256,0)) throw "Sending error!";
			if (!strcmp(buf,"exit")) break;
			
			msize = recv(server_sock,buf,256,0);
			if (SOCKET_ERROR == msize) throw "Receiving error!";
			buf[msize] = '\0';
			cout << buf << "\n";
		}
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
