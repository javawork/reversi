#include "StdAfx.h"
#include "SendPacket.h"
#include "common/packet.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

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


bool CSendPacket::PackProtoBuf(::google::protobuf::Message *pMsg)
{
	if( nullptr == pMsg )
		return false;

	int bufSize = pMsg->ByteSize();

	if( Protocol_Header_Total_Size+bufSize > MAX_PACKET_SIZE )
		return false;

	// pack
	::google::protobuf::io::ArrayOutputStream os(m_Buffer+Protocol_Header_Total_Size, bufSize);
	pMsg->SerializeToZeroCopyStream(&os);

	*(USHORT*)(m_Buffer) = bufSize;

	return true;
}