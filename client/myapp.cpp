#include "myapp.h"
#include "myframe.h"
#include "mysession.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
	_CrtDumpMemoryLeaks();

	session_ = NULL;
	session_ = new MySession();
	session_->connect( "192.168.0.163", 10000 );
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