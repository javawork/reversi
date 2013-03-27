#include "StdAfx.h"
#include "ReversySession.h"
#include "protocol.h"
#include "Room.h"

CRoom g_Room;			//юс╫ц


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
	case PROTOCOL_ID::C_Login:			Process_Login();			break;
	case PROTOCOL_ID::C_SetPiece:		Process_SetPiece();			break;
	};
}


void CReversySession::Process_Login()
{
	CSendPacket Packet;
	Packet.SetID( PROTOCOL_ID::S_Login );
	Send(Packet);

	g_Room.EnterSession(this);

	//if( true == g_Room.EnterSession(this) )
	//{
	//	return;
	//}
	
	//m_socket.close();
}

void CReversySession::Process_SetPiece()
{
	CSendPacket Packet;
	Packet.SetID( PROTOCOL_ID::S_SetPiece );
	Packet.AddBody(m_RecvPacket.GetBodyBuffer(), m_RecvPacket.GetBodySize());
	g_Room.BroadcastExceptMe(Packet, this);
}