#include "stdafx.h"
#include "ClientWsaModule.h"
#include "Object.h"
#include "Components.h"
#include "Scene.h"
#include "ServerScene.h"
#include "NetInputManagerComponent.h"

ClientWsaModule::ClientWsaModule()
{
	hWnd = NULL;
	sock = NULL;
	recvbytes = 0;
	sendbytes = 0;
	recvdelayed = false;
	isConnected = false;
	IPAddr = "127.0.0.1";
}

ClientWsaModule::~ClientWsaModule()
{
	Destroy();
}

void ClientWsaModule::Init(HWND a_hWnd, ServerScene* sp, string IP)
{
	int ret;
	hWnd = a_hWnd;
	scene = sp;
	if (NULL == hWnd)
	{
		cout << "Error hWnd is null\n";
		return;
	}

	cout << "Client WSAAsyncSelect init\n";
	ret = WSAStartup(MAKEWORD(2, 2), &WsaData);
	if (ret != 0)
	{
		wprintf(L"WSAStaryup Error error: %ld\n", WSAGetLastError());
		return;
	}

	//sock = ::socket(AF_INET, SOCK_STREAM, 0);
	sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == sock)
	{
		cout << "INVALID_SOCKET Error\n";
		WSACleanup();
		return;
	}

	ret = WSAAsyncSelect(sock, hWnd, WM_SOCKET, FD_READ | FD_WRITE | FD_CONNECT | FD_CLOSE);
	if (SOCKET_ERROR == ret)
	{
		cout << "WSAAsyncSelect Error\n";
		WSACleanup();
		closesocket(sock);
		return;
	}

	if (IP != "none")IPAddr = IP;
	cout << "IPAddress: " << IPAddr << "\n";
	cout << "Port: " << SERVER_PORT << "\n";

	memset(&s_address, 0, sizeof(s_address));
	s_address.sin_family = AF_INET;
	s_address.sin_port = htons(SERVER_PORT);
	cout << IPAddr.c_str() << "\n";
	s_address.sin_addr.S_un.S_addr = inet_addr(IPAddr.c_str());

	ret = connect(sock, reinterpret_cast<sockaddr*>(&s_address), sizeof(s_address));
	if (SOCKET_ERROR == ret) {
		wprintf(L"connect function failed with error: %ld\n", WSAGetLastError());
		if (WSAEWOULDBLOCK != WSAGetLastError()){
			/*
				WSAEWOULDBLOCK(10035) to be reported as the result 
				from calling connect on a nonblocking SOCK_STREAM socket,
				since some time must elapse for the connection to be established.
			*/
			closesocket(sock);
			WSACleanup();
		}
	}
}

void ClientWsaModule::Connect()
{
	if (!isConnected)
	{
		connect(sock, reinterpret_cast<sockaddr*>(&s_address), sizeof(s_address));
		Login();
	}
}

void ClientWsaModule::Destroy()
{
	cout << "Destroy()\n";
	closesocket(sock);
	WSACleanup();
}

void ClientWsaModule::Process_Packet(char* packet)
{
	switch (packet[1])
	{
	case S2C_LOGIN:
	{
		cout << "S2C_LOGIN\n";
		InitPlayer();
		isConnected = true;
		break;
	}
	case S2C_ENTER:
	{
		sc_packet_enter* p = reinterpret_cast<sc_packet_enter*>(packet);
		int id = p->oth_id;
		cout << "S2C_ENTER ["<<id<<"]\n";
		m_oth[id] = scene->EnterPlayer(id);
		
		cout << "id: " << p->oth_id << " entered\n";
		break;
	} 
	case S2C_MOVE:
	{
		sc_packet_move* p = reinterpret_cast<sc_packet_move*>(packet);
		int id = p->id;
		cout << "move [" << id << "]\n";
		if (id == m_id)
		{
			NetInputManagerComponent* nimc = m_player->FindComponent<NetInputManagerComponent>();
			if (nullptr != nimc)nimc->Move(p->x, p->y, p->z, p->rx, p->ry, p->rz);
			else { cout << "nimc is nullptr\n"; };
		}
		else
		{
			if (m_oth[id])
			{
				NetInputManagerComponent* nimc = m_oth[id]->FindComponent<NetInputManagerComponent>();
				if (nullptr != nimc)nimc->Move(p->x, p->y, p->z, p->rx, p->ry, p->rz);
				else { cout << "nimc is nullptr\n"; };

			//cout << "other move id: " << p->id << "\n";
			}
		}
		break;
	}
	case S2C_DISCONNECT:
	{
		sc_packet_disconnect* p = reinterpret_cast<sc_packet_disconnect*>(packet);
		cout << "id: " << p->id << " disconnected\n";
		break;
	}
	}
}

void ClientWsaModule::Test()
{
	cout << "TEST()\n";
	BasePacket p;
	p.size = sizeof(p);
	p.type = C2S_ERROR;
	send(sock, reinterpret_cast<char*>(&p), p.size, 0);
}

void ClientWsaModule::Login()
{
	if (isConnected) 
	{
		cout << "Already logined\n";
		return;
	}
	cout << "Login()\n";
	cs_packet_login p;
	p.size = sizeof(p);
	p.type = C2S_LOGIN;
	p.test = rand() % 100;
	send(sock, reinterpret_cast<char*>(&p), p.size, NULL);
}

void ClientWsaModule::InitPlayer()
{
	sc_packet_login_ok* p = reinterpret_cast<sc_packet_login_ok*>(packet_buf);
	m_id = p->id;

	m_player->m_id = m_id;
	cout << "id:" << m_player->m_id;
}

void ClientWsaModule::Move(float dirX, float dirZ, float rx)
{
	cs_packet_move p;
	p.size = sizeof(p);
	p.type = C2S_MOVE;
	p.dirX = dirX;
	p.dirZ = dirZ;
	p.rx = rx;

	send(sock, reinterpret_cast<char*>(&p), p.size, 0);
}

void ClientWsaModule::SetPlayer(Object* p)
{
	m_player = p;
}

void ClientWsaModule::ProcessSocketMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (WM_SOCKET == message)
	{
		switch (WSAGETSELECTEVENT(lParam))
		{
			case FD_READ:
			{
				int rest_byte = recv(sock, packet_buf, SCV::max_buf_size, 0);
				char* p = packet_buf;
				while (rest_byte > 0)
				{
					int packet_size = *p;
					Process_Packet(p);
					p += packet_size;
					rest_byte -= packet_size;
				}
				break;
			}
			case FD_WRITE:
			{
				if (isConnected)
				{

				}
				else Connect();
				//cout << "FD_WRITE\n";
				break;
			}
			case FD_CONNECT:
			{
				//cout << "FD_CONNECT\n";
				
				break;
			}
			case FD_CLOSE:
			{
				isConnected = false;
				cout << "Server Closed\n";
				//cout << "FD_CLOSE\n";
				break;
			}
			default:
			{

			}
		}
	}
}
