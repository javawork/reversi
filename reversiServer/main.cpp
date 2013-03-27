// ReversyServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ReversyServer.h"


int _tmain(int argc, _TCHAR* argv[])
{
	boost::asio::io_service io_service;
   
	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), 10000);
	ReversyServer_Ptr server(new CReversyServer(io_service, endpoint));

	std::cout<< "start server..." << std::endl;
	io_service.run();

	return 0;
}

