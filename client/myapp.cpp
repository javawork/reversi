#include "myapp.h"
#include "myframe.h"
#include "mysession.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
	//_CrtSetBreakAlloc(141);
	_CrtDumpMemoryLeaks();

	session_ = NULL;
	session_ = new MySession();

	const int host_size = 32;
	char host[host_size] = {0,};
	GetPrivateProfileStringA("Reversi", "host", "0.0.0.0", host, host_size, "./Client.ini");
	const int port = GetPrivateProfileIntA("Reversi", "port", 0, "./Client.ini");

	session_->connect( host, port );
    MyFrame * myframe = new MyFrame( wxT("reversi"), session_ );
	session_->set_listener( myframe );
    myframe->Show( true );

    return true;
}

int MyApp::OnExit()
{
	if ( session_ )
	{
		delete session_;
		session_ = NULL;
	}
	return 0;
}