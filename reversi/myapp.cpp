#include "myapp.h"
#include "myframe.h"

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    MyFrame *myframe = new MyFrame(wxT("reversi"));
    myframe->Show(true);

    return true;
}