#include "mysession.h"
#include <SDKDDKVer.h>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

using boost::asio::ip::tcp;

class MySession_Impl
{
public:
	MySession_Impl() 
		: socket(io_service)
		, read_buffer_(new char[max_length], max_length)
		, packet_listener_(NULL)
	{}

	~MySession_Impl()
	{
		delete [] boost::asio::buffer_cast<char*>(read_buffer_);
		//worker->interrupt();
		//worker->join();
		delete worker;
	}

	void connect( const char * ip, const int port )
	{
		boost::asio::ip::tcp::endpoint endpoint(
			boost::asio::ip::address::from_string(ip), port);

		socket.async_connect
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

		worker = new boost::thread(boost::bind(&boost::asio::io_service::run, &io_service));
	}

	void write(const short packet_code, const char * body, const size_t body_len)
	{
		PacketHeader * header = new PacketHeader();
		header->code = packet_code;
		header->size = static_cast<short>(body_len);

		boost::asio::const_buffer header_buffer(header, sizeof(PacketHeader));
		boost::asio::const_buffer body_buffer(body, body_len);
		std::vector<boost::asio::const_buffer> list;
		list.push_back(header_buffer);
		list.push_back(body_buffer);

		io_service.post
		(
			[this, list]()
			{
				this->do_write(list);
			}
		);
	}

	void set_listener(PacketListener * listener)
	{
		packet_listener_ = listener;
	}

protected:
	void do_read()
	{
		socket.async_read_some
		(
			boost::asio::buffer(read_buffer_),
			[this](const boost::system::error_code & error, std::size_t bytes_transferred)
			{
				if (false == error)
				{
					//std::cout << "do_read : " << bytes_transferred << std::endl;
					char* p1 = boost::asio::buffer_cast<char*>(read_buffer_);
					packet_listener_->OnReceive( p1, bytes_transferred );
					this->do_read();
				}
				else
				{
					delete this;
				}
			}
		);
	}

	template <typename ConstBufferSequence>
	void do_write( const ConstBufferSequence & buffer )
	{
		socket.async_write_some
		(
			buffer,
			[this, buffer](const boost::system::error_code & error, std::size_t bytes_transferred)
			{
				if (false == error)
				{
					std::cout << "do_write : " << bytes_transferred << std::endl;
					//delete [] boost::asio::buffer_cast<char*>(*buffer);
					//delete buffer;
				}
				else
				{
					delete this;
				}
			}
		);
	}

private:
	boost::asio::io_service io_service;
	tcp::socket socket;
	boost::thread * worker;
	boost::asio::mutable_buffer read_buffer_;
	PacketListener * packet_listener_;
	enum { max_length = 1024 };
};


MySession::MySession(void) : impl_(new MySession_Impl())
{
}


MySession::~MySession(void)
{
	delete impl_;
	impl_ = NULL;
}

void MySession::connect( const char * ip, const int port )
{
	impl_->connect(ip, port);
}

void MySession::write(const short packet_code, const char * body, const size_t body_len)
{
	impl_->write(packet_code, body, body_len);
}

void MySession::set_listener(PacketListener * listener)
{
	impl_->set_listener(listener);
}