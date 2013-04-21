#include "StdAfx.h"
#include "ReversySession.h"
#include "Common/Packet.pb.h"
#include "Room.h"

CRoom g_Room;			//임시


CReversySession::CReversySession(boost::asio::io_service& service)
	: m_socket(service)
{
	m_RecvPacket.Init();
}

CReversySession::~CReversySession()
{
}

void CReversySession::Recv_Header()
{
	boost::asio::async_read( m_socket, 
		boost::asio::buffer( m_RecvPacket.GetHeaderBuffer(), Protocol_Header_Total_Size ),
		boost::bind(&CReversySession::Handle_Recv_Header, shared_from_this(),
			boost::asio::placeholders::error));
}

void CReversySession::Recv_Body()
{
	boost::asio::async_read( m_socket, 
		boost::asio::buffer( m_RecvPacket.GetBodyBuffer(), m_RecvPacket.GetBodySize() ),
		boost::bind(&CReversySession::Handle_Recv_Body, shared_from_this(),
			boost::asio::placeholders::error));
}

void CReversySession::Send(CSendPacket& Packet)
{
	boost::asio::async_write( m_socket,
		boost::asio::buffer( Packet.GetPacket(), Packet.GetPacketSize() ),
		boost::bind(&CReversySession::Handle_Send, shared_from_this(),
			boost::asio::placeholders::error));
}

void CReversySession::Handle_Recv_Header(const boost::system::error_code &err)
{
	if(!err )
	{
		if(m_RecvPacket.CheckRecvHeader())
		{
			if( 0 == m_RecvPacket.GetBodySize() )
			{
				ProcessPacket();
				Recv_Header();
			}
			else
			{
				Recv_Body();
			}
			return;
		}

		m_socket.close();
	}

	Handle_Disconnect();
}

void CReversySession::Handle_Recv_Body(const boost::system::error_code &err)
{
	if(!err)
	{
		ProcessPacket();
		Recv_Header();
	}
	else
	{
		Handle_Disconnect();
	}

}

void CReversySession::Handle_Send(const boost::system::error_code &err)
{
	if(!err)
	{
	}
	else
	{
		Handle_Disconnect();
	}

}

void CReversySession::Handle_Disconnect()
{
	std::cout<< "session disconnected..." << std::endl;
	g_Room.LeaveSession(this);
}

void CReversySession::ProcessPacket()
{
	std::cout<< "Recv packet" << std::endl;
	switch(m_RecvPacket.GetId())
	{
	case packet::C_LOGIN:			Process_Login();			break;
	case packet::C_SET_PIECE:		Process_SetPiece();			break;
	};
}


void CReversySession::Process_Login()
{
	int iID = g_Room.EnterSession(this);
	if( -1 == iID )
	{
		//fail to enter the room
		m_socket.close();
		return;
	}

	packet::S_Login src_login;
	src_login.set_id(iID);			//id번호 뭘로?

	CSendPacket Packet;
	Packet.SetID( packet::S_LOGIN );
	Packet.PackProtoBuf( &src_login );
	
	Send(Packet);

	g_Room.TryStartGame();
	
}

void CReversySession::Process_SetPiece()
{
	packet::C_SetPiece SetPieceInfo;
	m_RecvPacket.UnpackProtoBuf(&SetPieceInfo);

	int iPosX = SetPieceInfo.pos_index() % BOARD_SIZE;
	int iPosY = SetPieceInfo.pos_index() / BOARD_SIZE;

	g_Room.SetPiece(iPosX, iPosY, this);
}