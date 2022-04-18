#pragma once
#include"../Headers/constants.h"
#include"../Headers/headers.h"

class Character
{
public:
	int m_currHp;
	float m_currReviveTime;
	bool m_bDead;
	bool m_bAutoRevive;
	float x, y, z;
	float rx, ry, rz;

public:
	Character();
	~Character();

	void PrintAll();
	void PrintPos();
};

