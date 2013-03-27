#include "StdAfx.h"
#include "ReversyServer.h"

CReversyServer::CReversyServer(boost::asio::io_service &service, boost::asio::ip::tcp::endpoint &endpoint)
	:m_Service(service), m_Accepter( service, endpoint )
{
	Accept();
}
	
void CReversyServer::Accept()
{
	ReversySession_Ptr NewSession(new CReversySession(m_Service));

	m_Accepter.async_accept( NewSession->GetSocket(), 
		boost::bind(&CReversyServer::Handle_Accept, this, NewSession,
		boost::asio::placeholders::error));

}

void CReversyServer::Handle_Accept(ReversySession_Ptr pSession, const boost::system::error_code& err)
{
	if( !err )
	{
		std::cout<< "session connected..." << std::endl;
		pSession->Recv_Header();
	}

	Accept();
}


