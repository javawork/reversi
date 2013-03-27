#include "StdAfx.h"
#include "SendPacket.h"


CSendPacket::CSendPacket(void)
{
	Init();
}


CSendPacket::~CSendPacket(void)
{
}

void CSendPacket::Init()
{
	memset(m_Buffer, 0, Protocol_Header_Total_Size);
}


bool CSendPacket::AddBody(const char* pBody, int iSize)
{
	if( GetPacketSize()+iSize > MAX_PACKET_SIZE )
		return false;

	memcpy(m_Buffer+GetPacketSize(), pBody, iSize);
	(*(USHORT*)m_Buffer) += iSize;

	return true;
}