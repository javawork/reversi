#pragma once

#include "RecvPacket.h"
#include "SendPacket.h"

class CReversySession : public boost::enable_shared_from_this<CReversySession>
{
public:
	CReversySession(boost::asio::io_service& service);
	~CReversySession();

	boost::asio::ip::tcp::socket& GetSocket();

	void Recv_Header();
	void Recv_Body();
	void Send(CSendPacket& Packet);

	void Handle_Recv_Header(const boost::system::error_code &err);
	void Handle_Recv_Body(const boost::system::error_code &err);
	void Handle_Send(const boost::system::error_code &err);
	void Handle_Disconnect();

	void ProcessPacket();
	void Process_Login();
	void Process_SetPiece();
protected:
	boost::asio::ip::tcp::socket m_socket;
	CRecvPacket m_RecvPacket;
	
};

typedef boost::shared_ptr<CReversySession> ReversySession_Ptr;

inline boost::asio::ip::tcp::socket& CReversySession::GetSocket()
{
	return m_socket;
}

