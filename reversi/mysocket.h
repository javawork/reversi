#pragma once

struct MySocket_Pimpl;

class MySocket
{
public:
	MySocket(void);
	~MySocket(void);

	bool connect( const char * host, const char * port );
	void send_login();
	void recv_login();
	void recv_startgame();
	void send_setpiece(const int index);
	int recv_setpiece();

	void set_recv_timeout();

private:
	MySocket_Pimpl * pimpl_;
};

