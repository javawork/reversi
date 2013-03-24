#include "mysocket.h"
#include <SDKDDKVer.h>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

struct MySocket_Pimpl
{
	MySocket_Pimpl() : s(io_service)
	{}

	boost::asio::io_service io_service;
	tcp::socket s;
};

struct PacketHeader
{
	unsigned short len;
	unsigned short id;
};

struct SetPiece
{
	PacketHeader header;
	int index;
};

enum PacketID
{
	C_LOGIN = 1,
	S_LOGIN,
	S_STARTGAME,
	C_SETPIECE,
	S_SETPIECE
};

MySocket::MySocket(void) : pimpl_(new MySocket_Pimpl())
{
}


MySocket::~MySocket(void)
{
	delete pimpl_;
	pimpl_ = NULL;
}

bool MySocket::connect( const char * host, const char * port )
{
	tcp::resolver resolver(pimpl_->io_service);
	tcp::resolver::query query(tcp::v4(), host, port);
	tcp::resolver::iterator iterator = resolver.resolve(query);

	boost::asio::connect(pimpl_->s, iterator);
	return true;
}

void MySocket::send_login()
{
	PacketHeader header;
	header.len = 0;
	header.id = C_LOGIN;

	size_t ll = boost::asio::write(pimpl_->s, boost::asio::buffer(&header, sizeof(PacketHeader)));
	
}

void MySocket::recv_login()
{
	PacketHeader header;
	size_t reply_length = boost::asio::read(pimpl_->s, boost::asio::buffer(&header, sizeof(PacketHeader)));
}

void MySocket::recv_startgame()
{
	PacketHeader header;
	size_t reply_length = boost::asio::read(pimpl_->s, boost::asio::buffer(&header, sizeof(PacketHeader)));
}

void MySocket::send_setpiece(const int index)
{
	SetPiece pkt;
	pkt.header.len = sizeof(int);
	pkt.header.id = C_SETPIECE;
	pkt.index = index;

	size_t ll = boost::asio::write(pimpl_->s, boost::asio::buffer(&pkt, sizeof(SetPiece)));
}

int MySocket::recv_setpiece()
{
	SetPiece pkt;
	size_t reply_length = boost::asio::read(pimpl_->s, boost::asio::buffer(&pkt, sizeof(SetPiece)));
	if (reply_length <= 0)
		return -1;
	return pkt.index;
}

void MySocket::set_recv_timeout()
{
	int32_t timeout = 1000;
    setsockopt(pimpl_->s.native(), SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    //setsockopt(socket.native(), SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
}