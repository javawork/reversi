#pragma once
#include "ReversySession.h"

class CReversyServer
{
public:
	CReversyServer(boost::asio::io_service &service, boost::asio::ip::tcp::endpoint &endpoint);
	
	void Accept();
	
	void Handle_Accept(ReversySession_Ptr pSession, const boost::system::error_code& err);

protected:
	boost::asio::io_service &m_Service;
	boost::asio::ip::tcp::acceptor	m_Accepter;
};

typedef boost::shared_ptr<CReversyServer> ReversyServer_Ptr;

