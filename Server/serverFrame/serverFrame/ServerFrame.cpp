#include"ServerFrame.h"

void ServerFrame::Init()
{
	int ret;
	cout << "server Init";
	ret=WSAStartup(MAKEWORD(2, 2), &WsaData);
	l_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN s_address;
	memset(&s_address, 0, sizeof(s_address));
	s_address.sin_family = AF_INET;
	s_address.sin_port = htons(SERVER_PORT);
	s_address.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	ret = ::bind(l_socket, reinterpret_cast<sockaddr*>(&s_address), sizeof(s_address));
	if (SOCKET_ERROR == ret)
	{
		cout << "Bind Error\n";
		cout << WSAGetLastError() << "\n";
	}

	ret = listen(l_socket, SOMAXCONN);
	if (SOCKET_ERROR == ret)
	{
		cout << "Listen Error\n";
		cout << WSAGetLastError() << "\n";
	}

	// Create IOCP
	g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);

	// Bind listen socket to iocp
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(l_socket), g_iocp, 999, 0);

	AcceptMy();
	cout << " - ok\n";
}

void ServerFrame::Run()
{
	cout << "server Run\n";
	for (int i = 0; i < 4; ++i)
		worker_threads.emplace_back(&ServerFrame::worker_thread, this, i);

	for (auto& wt : worker_threads)
		wt.join();
}

void ServerFrame::Destroy()
{
	cout << "Destroy\n";
	WSACleanup();
}

void ServerFrame::Test()
{
	cout << "Call Test()\n";

}

void ServerFrame::worker_thread(int i)
{
	while (true)
	{
		DWORD io_byte;
		ULONG_PTR key;
		WSAOVERLAPPED* over;
		//cout << "GetQueuedCompletionStatus\n";
		GetQueuedCompletionStatus(g_iocp, &io_byte, &key, &over, INFINITE);

		EXOVER* exover = reinterpret_cast<EXOVER*>(over);
		int user_id = static_cast<int>(key);
		switch (exover->op)
		{
		case Op::recv:
			OP_recv(io_byte, user_id, exover);
			break;
		case Op::send:
			OP_send(io_byte, user_id, exover);
			break;
		case Op::accept:
			OP_accept(io_byte, user_id, exover);
			break;
		}
	}
}

void ServerFrame::OP_send(DWORD io_byte, int user_id, EXOVER* exover)
{
	//cout << "OP_SEND\n";
	if (io_byte == 0)disconnect(user_id);
	delete exover;
}

void ServerFrame::OP_recv(DWORD io_byte, int user_id, EXOVER* exover)
{
	//cout << "OP_RECV\n";
	if (io_byte == 0) disconnect(user_id);
	else 
	{
		Client* cl = clients[user_id];
		DWORD flag = 0;
		Assemble_packet(io_byte, user_id);
		ZeroMemory(&cl->exover.over, sizeof(cl->exover.over));
		WSARecv(cl->sock, &cl->exover.wsabuf, 1, NULL, &flag, &cl->exover.over, NULL);
	}
}

void ServerFrame::OP_accept(DWORD io_byte, int user_id, EXOVER* exover)
{
	cout << "OP_ACCEPT\n";	
	int cl_id = -1;
	for (int i = 0; i < SCV::max_user; ++i)
	{
		Client* cl = clients[i];
		if (ClStatus::free == cl->cl_status)
		{
			cl->mtx_status.lock();
			cl->cl_status = ClStatus::activated;
			cl->mtx_status.unlock();
			cl->id = i;
			cl_id = i;
			cout << "Accept id[" << cl_id << "]\n";
			break;
		}
	}
	SOCKET c_socket = exover->c_socket;
	if (-1 == cl_id)closesocket(c_socket);
	else {
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(c_socket), g_iocp, cl_id, 0);
		Client* nc = clients[cl_id];
		nc->prev_size = 0;
		nc->exover.op = Op::recv;
		ZeroMemory(&nc->exover.over, sizeof(nc->exover.over));
		nc->exover.wsabuf.buf = nc->exover.io_buf;
		nc->exover.wsabuf.len = SCV::max_buf_size;

		nc->sock = c_socket;
		DWORD flags = 0;
		WSARecv(c_socket, &nc->exover.wsabuf, 1, NULL, &flags, &nc->exover.over, NULL);
	}
	AcceptMy();
}

void ServerFrame::Assemble_packet(DWORD io_byte, int user_id)
{
	Client* cl = clients[user_id];
	EXOVER ro = cl->exover;

	int rest_byte = io_byte;
	char* p = ro.io_buf;
	int packet_size = 0;

	if (0 != cl->prev_size)packet_size = cl->packet_buf[0];
	while (rest_byte > 0)
	{
		if (0 == packet_size)packet_size = p[0];
		if (packet_size <= rest_byte + cl->prev_size)
		{
			int diff = packet_size - cl->prev_size;
			memcpy(cl->packet_buf + cl->prev_size, p, diff);
			p += diff;
			rest_byte -= diff;
			packet_size = 0;
			Process_packet(user_id, cl->packet_buf);
			cl->prev_size = 0;
		}
		else 
		{
			memcpy(cl->packet_buf + cl->prev_size, p, rest_byte);
			cl->prev_size += rest_byte;
			rest_byte = 0;
		}
	}
}

void ServerFrame::Process_packet(int user_id, char* buf)
{
	switch (buf[1])
	{
	case C2S_LOGIN:
	{
		cs_packet_login* p = reinterpret_cast<cs_packet_login*>(buf);
		cout << p->test << "\n";
		Send_login_ok_packet(user_id);
		EnterGame(user_id);
		break;
	}
	case C2S_MOVE:
	{
		//cout << "C2S_MOVE  ";
		ProcessMove(user_id, buf);
		break;
	}
	case C2S_ERROR:
	{
		cout << "C2S_ERROR\n";
		disconnect(user_id);
		break;
	}
	default:
		cout << "Unknon packet type\n";
	}
}

inline bool ServerFrame::AcceptMy()
{
	accept_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	ZeroMemory(&accept_over.over, sizeof(accept_over.over));
	accept_over.op = Op::accept;
	accept_over.c_socket = accept_socket;
	return AcceptEx(l_socket, accept_socket, accept_over.io_buf, NULL,
		sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
		NULL, &accept_over.over);
}

void ServerFrame::SendPacket(int user_id, void* packet)
{
	char* buf = reinterpret_cast<char*>(packet);
	Client* cl = clients[user_id];

	EXOVER* exover = new EXOVER;
	exover->op = Op::send;
	exover->set(buf[0]);
	memcpy(exover->io_buf, buf, buf[0]);
	WSASend(cl->sock, &exover->wsabuf, 1, NULL, 0, &exover->over, NULL);
}

void ServerFrame::Send_login_ok_packet(int user_id)
{
	cout << "Send_login_ok_packet " << user_id << "\n";
	Client* cl = clients[user_id];
	Character& ch = cl->m_character;
	sc_packet_login_ok p;
	p.size = sizeof(sc_packet_login_ok);
	p.type = S2C_LOGIN;
	p.id = user_id;

	p.m_currHp = ch.m_currHp;
	p.m_currReviveTime = ch.m_currReviveTime;
	p.m_bDead = ch.m_bDead;
	p.m_bAutoRevive = ch.m_bAutoRevive;
	p.x = ch.x;
	p.y = ch.y;
	p.z = ch.z;
	p.rx = ch.rx;
	p.ry = ch.ry;
	p.rz = ch.rz;

	SendPacket(user_id, &p);
}

void ServerFrame::Send_enter_packet(int user_id, int oth_id)
{
	cout << "Send_enter_packet " << user_id << " " << oth_id << "\n";
	Client* cl = clients[oth_id];
	Character& ch = cl->m_character;

	sc_packet_enter p;
	p.size = sizeof(sc_packet_enter);
	p.type = S2C_ENTER;
	p.oth_id = oth_id;
	p.m_currHp = ch.m_currHp;
	p.x = ch.x;
	p.y = ch.y;
	p.z = ch.z;
	p.rx = ch.rx;
	p.ry = ch.ry;
	p.rz = ch.rz;

	SendPacket(user_id, &p);
}

void ServerFrame::EnterGame(int enter_id)
{
	for (int i = 0; i < SCV::max_user; ++i)
	{
		if (enter_id != i &&
			clients[i]->cl_status == ClStatus::activated) {
			Send_enter_packet(i, enter_id);
			Send_enter_packet(enter_id, i);
		}
	}
}

void ServerFrame::ProcessMove(int user_id, char* buf)
{
	Client* cl = clients[user_id];
	Character& character = cl->m_character;
	cs_packet_move* mv = reinterpret_cast<cs_packet_move*>(buf);

	// collide Check
	sc_packet_move m;
	m.size = sizeof(m);
	m.type = S2C_MOVE;
	m.id = user_id;
	m.x = 0;
	m.y = 0;
	m.z = 0;

	if (0 < mv->dirX)
	{
		m.x = 1;
	}
	if (0 > mv->dirX)
	{
		m.x = -1;
	}
	if (0 < mv->dirZ)
	{
		m.z = 1;
	}
	if (0 > mv->dirZ)
	{
		m.z = -1;
	}
	m.rx = 0;
	m.ry = 0;
	m.rz = 0;

	// check who move
	for (int i = 0; i < SCV::max_user; ++i)
	{
		if (clients[i]->cl_status == ClStatus::activated) {
			SendPacket(i, &m);
		}
	}
}

void ServerFrame::disconnect(int user_id)
{
	cout << "disconnet[" << user_id << "]\n";
	Client* cl = clients[user_id];
	cl->mtx_status.lock();
	cl->cl_status = ClStatus::free;
	cl->mtx_status.unlock();
	cl->m_character = Character();
	
	sc_packet_disconnect p;
	p.size = sizeof(p);
	p.type = S2C_DISCONNECT;
	p.id = user_id;
	for (int i = 0; i < SCV::max_user; ++i)
	{
		if (i!= user_id&&
			clients[i]->cl_status == ClStatus::activated) {
			cout << "send disconnect to " << i << "\n";
			SendPacket(i, &p);
		}
	}
}

ServerFrame::ServerFrame()
{
	clients.resize(SCV::max_user);
	for (int i = 0; i < SCV::max_user; ++i)
		clients[i] = new Client();
	WsaData = WSADATA();
	g_iocp = HANDLE();
	l_socket = SOCKET();
	accept_socket = SOCKET();
	accept_over = EXOVER();
}

ServerFrame::~ServerFrame()
{
	Destroy();
	for (int i = 0; i < SCV::max_user; ++i)
		delete clients[i];
}
