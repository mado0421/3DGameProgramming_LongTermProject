#pragma once
namespace SCV {
	const int max_user = 1000;
	const int max_buf_size = 1024;
	const int max_packet_size = 255;
}

enum class Op :int
{
	recv,
	send,
	accept
};

enum class ClStatus {
	free,
	allocated,
	activated,
	sleep,
};


constexpr int maxHp = 10;
constexpr float reviveTime = 3.0f;
