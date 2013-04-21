#pragma once

class PacketListener
{
public:
	virtual void OnReceive(const char * buffer, const size_t len) = 0;
};