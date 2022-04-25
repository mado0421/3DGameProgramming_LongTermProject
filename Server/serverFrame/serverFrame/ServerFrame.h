#pragma once
#include"Headers/headers.h"
#include"Headers/exover.h"
#include"Headers/constants.h"
#include"Client/Client.h"
#include"Packets/packet.h"
#include"DirectX/TransformComponent.h"

class ServerFrame
{
	WSADATA WsaData;
	HANDLE g_iocp;
	SOCKET l_socket;

	SOCKET accept_socket;
	EXOVER accept_over;
	vector<Client*> clients;

	mutex m;

	vector<thread> worker_threads;
public:
	void Init();
	void Run();
	void Destroy();
	void Test();

private:
	void worker_thread(int i);
	void OP_send(DWORD io_byte, int user_id, EXOVER* exover);
	void OP_recv(DWORD io_byte, int user_id, EXOVER* exover);
	void OP_accept(DWORD io_byte, int user_id, EXOVER* exover);

	void Assemble_packet(DWORD io_byte, int user_id);
	void Process_packet(int user_id,char* packet);
	inline bool AcceptMy();

private:
	void SendPacket(int user_id,void* packet);
	void Send_login_ok_packet(int user_id);
	void Send_enter_packet(int user_id, int oth_id);

private:
	void EnterGame(int enter_id);
	void ProcessMove(int user_id, char* buf);

private:
	void disconnect(int user_id);

public:
	ServerFrame();
	~ServerFrame();
};



// To do
/*
	connect
	->
	accept 
		accept 하고 등록하여 관리할
		클라이언트 class


*/
