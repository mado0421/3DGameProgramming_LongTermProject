#pragma once
namespace SCV {
	const int max_user = 1000;
	const int max_buf_size = 1024;
	const int max_packet_size = 255;
}
enum class Op : int
{
	recv,
	send,
	accept
};

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