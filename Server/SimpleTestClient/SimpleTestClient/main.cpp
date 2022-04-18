#include<iostream>
#pragma warning(disable:4996)

#include"../../serverFrame/serverFrame/Headers/headers.h"
#include"../../serverFrame/serverFrame/Packets/packet.h"
#include"../../serverFrame/serverFrame/Headers/constants.h"
#include"../../serverFrame/serverFrame/Client/Client.h"


using namespace std;

int main()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cout << "WSA Error\n";
	}
	
	SOCKET c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (c_socket == INVALID_SOCKET)
	{
		cout << "Socket Error\n";
		cout << WSAGetLastError() << "\n";
	}
	SOCKADDR_IN s_address;
	memset(&s_address, 0, sizeof(s_address));
	s_address.sin_family = AF_INET;
	s_address.sin_port = htons(SERVER_PORT);
	s_address.sin_addr.S_un.S_addr= inet_addr("127.0.0.1");
	
	if (SOCKET_ERROR !=
		connect(c_socket, reinterpret_cast<sockaddr*>(&s_address),
			sizeof(s_address)))
	{
		cout << "connect Success\n";
		int flag = 0;
		char buf[SCV::max_packet_size];
		cs_packet_login pack;
		pack.size = sizeof(pack);
		pack.type = C2S_LOGIN;
		srand(time(NULL));
		pack.test = 11220000 + rand() % 100;
		send(c_socket, (char*)&pack, pack.size, flag);


		recv(c_socket, buf, sizeof(buf), flag);
		sc_packet_login_ok* p = reinterpret_cast<sc_packet_login_ok*>(buf);
		cout << p->id << "  " << (int)p->size << "\n";
		while (true)
		{
			char command;
			cin >> command;

			switch (command)
			{
			case 'l':
			case'L':
				

				break;
			case 's':
			{
				cs_packet_move p;
				p.size = sizeof(p);
				p.type = C2S_MOVE;
				p.x = 0;
				p.y = 0;
				send(c_socket, (char*)&p, p.size, flag);
				break;
			}
			default:
				cout << "not command\n";
			}
			
		}
	}
	else
	{
		cout << "connect error\n";
		cout << WSAGetLastError() << "\n";
	}
	closesocket(c_socket);
	WSACleanup();
}