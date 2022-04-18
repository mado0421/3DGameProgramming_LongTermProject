#pragma once
#include"../Headers/headers.h"
#include"../Headers/exover.h"
#include"../Character/Character.h"


class Client
{
public:
	int id;

	atomic<ClStatus> cl_status;
	mutex mtx_status;

	SOCKET sock;
	EXOVER exover;
	int prev_size;
	char packet_buf[SCV::max_buf_size];

	Character m_character;

	// more data;

public:
	Client();


};

