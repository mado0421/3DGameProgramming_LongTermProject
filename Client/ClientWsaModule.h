#pragma once
#include"stdafx.h"
class ClientWsaModule
{
	WSADATA WsaData;
	HWND hWnd;
	string IPAddr;
	SOCKADDR_IN s_address;

	SOCKET sock;
	char packet_buf[SCV::max_buf_size];

	int recvbytes;
	int sendbytes;
	bool recvdelayed;
	bool isConnected;

// Player data;
	int m_id;
	/*Character m_character;
	unordered_map<int, Character> m_oth;*/

public:
	ClientWsaModule();
	virtual ~ClientWsaModule();

public:
	void Init(HWND hWnd, string IP = "none");
	void Process_Packet();
	void Connect();
	void Destroy();

public:
	void Test();
	void Login();

	void InitPlayer ();
	void Move(float x,float y,float z, float rx, float ry, float rz);

public:
	void ProcessSocketMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

};

