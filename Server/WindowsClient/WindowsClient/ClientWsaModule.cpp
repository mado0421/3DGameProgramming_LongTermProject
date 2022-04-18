#include "ClientWsaModule.h"

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

void ClientWsaModule::Init(HWND a_hWnd, string IP)
{
	int ret;
	hWnd = a_hWnd;
	if (NULL == hWnd)
	{
		cout << "Error hWnd is null\n";
		return;
	}

	cout << "Client WSAAsyncSelect init\n";
	ret = WSAStartup(MAKEWORD(2, 2), &WsaData);
	if (ret != 0)
	{
		cout << "WSAStaryup Error\n";
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

void ClientWsaModule::Process_Packet()
{
	switch (packet_buf[1])
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
		cout << "S2C_ENTER\n";
		sc_packet_enter* p = reinterpret_cast<sc_packet_enter*>(packet_buf);
		Character ch;
		ch.m_currHp = p->m_currHp;
		ch.x = p->x;
		ch.y = p->y;
		ch.z = p->z;
		ch.rx = p->rx;
		ch.ry = p->ry;
		ch.rz = p->rz;
		
		cout << "id: " << p->oth_id << " entered\n";
		ch.PrintAll();
		m_oth[p->oth_id] = ch;

		break;
	} 
	case S2C_MOVE:
	{
		sc_packet_move* p = reinterpret_cast<sc_packet_move*>(packet_buf);
		cout << "mv\n";
		if (p->id == m_id)
		{
			m_character.x = p->x;
			m_character.y = p->y;
			m_character.z = p->z;
			m_character.rx = p->rx;
			m_character.ry = p->ry;
			m_character.rz = p->rz;
			m_character.PrintPos();
		}
		else
		{
			Character& oth = m_oth[p->id];
			oth.x = p->x;
			oth.y = p->y;
			oth.z = p->z;
			oth.rx = p->rx;
			oth.ry = p->ry;
			oth.rz = p->rz;
			//cout << "other move id: " << p->id << "\n";
			oth.PrintPos();
		}
		break;
	}
	case S2C_DISCONNECT:
	{
		sc_packet_disconnect* p = reinterpret_cast<sc_packet_disconnect*>(packet_buf);
		m_oth.erase(p->id);
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
	m_character.m_currHp = p->m_currHp;
	m_character.m_currReviveTime = p->m_currReviveTime;
	m_character.m_bDead = p->m_bDead;
	m_character.m_bAutoRevive = p->m_bAutoRevive;
	m_character.x = p->x;
	m_character.y = p->y;
	m_character.z = p->z;
	m_character.rx = p->rx;
	m_character.ry = p->ry;
	m_character.rz = p->rz;

	m_character.PrintAll();
}

void ClientWsaModule::Move(float x, float y, float z, float rx, float ry, float rz)
{
	cout << "Move (" << x << "," << y << "," << z << ") " << rx << " " << ry << " " << rz << "\n";
	cs_packet_move p;
	p.size = sizeof(p);
	p.type = C2S_MOVE;
	p.x = x;
	p.y = y;
	p.z = z;
	p.rx = rx;
	p.ry = ry;
	p.rz = rz;
	send(sock, reinterpret_cast<char*>(&p), p.size, 0);
}

void ClientWsaModule::ProcessSocketMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (WM_SOCKET == message)
	{
		switch (WSAGETSELECTEVENT(lParam))
		{
			case FD_READ:
			{
				cout << "FD_READ\n";
				recv(sock, packet_buf, SCV::max_buf_size, 0);
				Process_Packet();
				break;
			}
			case FD_WRITE:
			{
				if (isConnected)
				{

				}
				else Connect();
				cout << "FD_WRITE\n";
				break;
			}
			case FD_CONNECT:
			{
				cout << "FD_CONNECT\n";
				
				break;
			}
			case FD_CLOSE:
			{
				isConnected = false;
				cout << "FD_CLOSE\n";
				break;
			}
			default:
			{

			}
		}
	}
}
