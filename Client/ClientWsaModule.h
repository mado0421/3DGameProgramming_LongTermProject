#pragma once
#include"stdafx.h"
class Object;
class Scene;
class ServerScene;
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
	Object* m_player;
	unordered_map<int, Object*> m_oth;

	ServerScene* scene;

public:
	ClientWsaModule();
	virtual ~ClientWsaModule();

public:
	void Init(HWND hWnd, ServerScene* sp, string IP = "none");
	void Process_Packet(char* packet);
	void Connect();
	void Destroy();

public:
	void Test();
	void Login();

	void InitPlayer ();
	void Move(float dirX,float dirZ,float rx);
	void SetPlayer(Object* p);
	int GetPlayerId() { return m_id; }

public:
	void ProcessSocketMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

};

