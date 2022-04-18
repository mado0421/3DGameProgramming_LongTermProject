#pragma once
#include<WS2tcpip.h>
#include"constants.h"

struct EXOVER {
	WSAOVERLAPPED over;
	Op	op;
	char io_buf[SCV::max_buf_size];
	union {
		WSABUF wsabuf;
		SOCKET c_socket;
		int p_id;
	};
	void set(int len)
	{
		ZeroMemory(&over, sizeof(over));
		wsabuf.buf = io_buf;
		wsabuf.len = len;
	}
};