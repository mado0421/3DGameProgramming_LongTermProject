#pragma once
#pragma pack(push,1)
struct BasePacket
{
	char size;
	char type;
};

struct sc_packet_login_ok : BasePacket
{
	int id;

	//Character status
	int m_currHp;
	float m_currReviveTime;
	bool m_bDead;
	bool m_bAutoRevive;
	float x, y, z;
	float rx, ry, rz;
};

struct sc_packet_enter : BasePacket
{
	int oth_id;
	float x, y, z;
	float rx, ry, rz;
	int m_currHp;
};

struct sc_packet_disconnect : BasePacket
{
	int id;
};

struct sc_packet_move : BasePacket
{
	int id;
	float x, y, z;
	float rx, ry, rz;
};

struct cs_packet_login : BasePacket
{
	int test;
};
struct cs_packet_move : BasePacket
{
	float dirX, dirZ;
	float rx;
};

struct cs_packet_fire : BasePacket
{

};

#define C2S_ERROR	-1
#define C2S_LOGIN	1
#define C2S_MOVE	2

#define S2C_ERROR	-1
#define S2C_LOGIN	1
#define S2C_ENTER	2
#define S2C_MOVE	3
#define S2C_DISCONNECT 4

#pragma pack(pop)