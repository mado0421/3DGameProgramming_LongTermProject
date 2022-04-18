#include "Client.h"

Client::Client()
{
	id = -1;
	cl_status = ClStatus::free;
	id = NULL;
	exover = EXOVER();
	prev_size = 0;

	m_character = Character();
}