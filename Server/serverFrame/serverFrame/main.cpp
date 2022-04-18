#include"ServerFrame.h"

int main()
{
	ServerFrame server;
	server.Init();
	server.Run();
	server.~ServerFrame();
}