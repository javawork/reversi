#include "StdAfx.h"
#include "RecvPacket.h"


CRecvPacket::CRecvPacket(void)
{
	Init();
}


CRecvPacket::~CRecvPacket(void)
{
}

void CRecvPacket::Init()
{
}

bool CRecvPacket::CheckRecvHeader()		//check recv size header
{
	if( GetBodySize() > MAX_PACKET_SIZE-Protocol_Header_Total_Size )
		return false;

	//check Id range.

	return true;
}

