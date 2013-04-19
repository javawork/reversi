#include "mysession.h"
#include <SDKDDKVer.h>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include "packetlistener.h"

using boost::asio::ip::tcp;

struct MySession_Impl
{
	MySession_Impl() 
		: socket(io_service)
		, read_buffer(new char[max_buffer_size], max_buffer_size)
		, worker(NULL)
	{}

	~MySession_Impl()
	{
		if (worker)
		{
			io_service.stop();
			delete worker;
			worker = NULL;
		}
		
		delete [] boost::asio::buffer_cast<char*>(read_buffer);
	}

	boost::asio::io_service io_service;
	tcp::socket socket;
	boost::thread * worker;
	boost::asio::mutable_buffer read_buffer;
	enum { max_buffer_size = 1024 };
};

MySession::MySession() 
	: impl_(new MySession_Impl())
	, listener_(NULL)
{
}


MySession::~MySession()
{
	if (impl_)
	{
		delete impl_;
		impl_ = NULL;
	}
}

void MySession::set_listener(PacketListener * listener)
{
	listener_ = listener;
}

void MySession::connect( const char * ip, const int port )
{
	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(ip), port);

	impl_->socket.async_connect
	(
		endpoint,
		[this]( const boost::system::error_code & error )
		{
			if ( false == error )
			{
				std::cout << "connected" << std::endl;
				this->do_read();
			}
			else
			{
				delete this;
			}
		}
	);

	impl_->worker = new boost::thread(boost::bind(&boost::asio::io_service::run, &impl_->io_service));
}

void MySession::write(const char * p, const size_t len)
{
	char * write_ptr = new char[len];
	memcpy(write_ptr, p, len);

	impl_->io_service.post
	(
		[this, write_ptr, len]()
		{
			this->do_write(write_ptr, len);
		}
	);
}

void MySession::do_read()
{
	impl_->socket.async_read_some
	(
		boost::asio::buffer(impl_->read_buffer),
		[this](const boost::system::error_code & error, std::size_t bytes_transferred)
		{
			if (false == error)
			{
				if (listener_)
				{
					const char * read_ptr = boost::asio::buffer_cast<char*>(impl_->read_buffer);
					listener_->OnReceive( read_ptr, bytes_transferred );
				}
				this->do_read();
			}
			else
			{
				delete this;
			}
		}
	);
}

void MySession::do_write( const char * p, const size_t len )
{
	impl_->socket.async_write_some
	(
		boost::asio::buffer(p, len),
		[this, p](const boost::system::error_code & error, std::size_t bytes_transferred)
		{
			if (false == error)
			{
				std::cout << "do_write : " << bytes_transferred << std::endl;
				delete [] p;
			}
			else
			{
				delete this;
			}
		}
	);
}