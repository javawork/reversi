#pragma once

#include "packetlistener.h"

class MySession_Impl;

class MySession
{
public:
	MySession(void);
	~MySession(void);

	void connect( const char * ip, const int port );
	void write(const short packet_code, const char * body, const size_t body_len);

	void set_listener(PacketListener * listener);

private:
	MySession_Impl * impl_;
};

