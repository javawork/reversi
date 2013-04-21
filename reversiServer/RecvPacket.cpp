#include "StdAfx.h"
#include "RecvPacket.h"
#include "common/packet.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

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

bool CRecvPacket::UnpackProtoBuf(::google::protobuf::Message *pMsg)
{
	if( nullptr == pMsg )
		return false;

	if( GetBodySize() <= 0 )
		return false;

	::google::protobuf::io::ArrayInputStream is(GetBodyBuffer(), GetBodySize());
	pMsg->ParseFromZeroCopyStream(&is);

	return true;
}
