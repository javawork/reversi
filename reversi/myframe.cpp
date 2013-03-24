#include "myframe.h"
#include "mysocket.h"
#include <boost/date_time.hpp>


static wxColour Color_Green = wxColour( 34, 139, 34 );
static wxColour Color_Khaki = wxColour( 240, 230, 140 );
static wxColour Color_Red = wxColour( 255, 0, 0 );
static wxColour Color_Blue = wxColour( 0, 0, 255 );
static wxColour Color_Black = wxColour( 0, 0, 0 );

const int TIMER_ID = 101;

static void ShowMessageBox( const wxString& message )
{
	wxMessageDialog dlg( NULL, message, "reversi", wxOK);
	dlg.ShowModal();
	dlg.Destroy();
}


MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(380, 400))
	   , timer_(this, TIMER_ID)
{
	/*wxPanel *panel = new wxPanel(this, wxID_ANY);

	wxButton *button = new wxButton(panel, wxID_EXIT, wxT("Quit"), wxPoint(20, 20));
	Connect(wxID_EXIT, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Simple::OnQuit));
	Centre();*/
	board_.init();
	this->Connect(wxEVT_PAINT, wxPaintEventHandler(MyFrame::OnPaint));
	this->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(MyFrame::OnClick));
	//this->Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(MyFrame::OnClick_));
	this->Connect(wxEVT_TIMER, wxTimerEventHandler(MyFrame::OnTimer));
	
	socket_ = new MySocket();
	socket_->connect("192.168.0.69", "10000");
	socket_->send_login();
	socket_->recv_login();
	ShowMessageBox("Login");

	socket_->recv_startgame();
	//socket_->set_recv_timeout();

	ShowMessageBox("Start Game");
	timer_.Start(2000);
	m_Thread = boost::thread(&MyFrame::recv_thread, this);  
	m_Thread.detach();
}

void MyFrame::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	const SquareList & square_list = board_.get_square_list();
	this->draw_line(dc, square_list);
	this->draw_piece(dc, square_list);
}

void MyFrame::draw_line(wxPaintDC & dc, const SquareList & list)
{
	int start_x = size_square;
	int start_y = size_square;
	SquareList::const_iterator iter = list.begin();
	for ( ; iter != list.end(); ++iter)
	{
		Square s = (*iter);
		if (s.index%max_square == max_square-1)
		{
			dc.DrawLine( wxPoint(0, start_y), wxPoint(s.x, s.y) );
			start_y += size_square;
		}
		else if (s.index > 28)
		{
			dc.DrawLine( wxPoint(start_x, 0), wxPoint(s.x, s.y) );
			start_x += size_square;
		}

		dc.DrawLine( wxPoint(size_square*max_square, 0), wxPoint(size_square*max_square, size_square*max_square) );
	}
}

void MyFrame::draw_piece(wxPaintDC & dc, const SquareList & list)
{
	SquareList::const_iterator iter = list.begin();
	for ( ; iter != list.end(); ++iter)
	{
		Square s = (*iter);
		if ( s.own == Piece_None)
		{
			continue;
		}
		else if ( s.own == Piece_White)
		{
			dc.SetBrush( Color_Blue );
			dc.DrawCircle(s.x-(size_square/2), s.y-(size_square/2), 20);
		}
		else if ( s.own == Piece_Dark)
		{
			dc.SetBrush( Color_Red );
			dc.DrawCircle(s.x-(size_square/2), s.y-(size_square/2), 20);
		}
	}
}

void MyFrame::OnClick(wxMouseEvent & event)
{
	const int x = event.GetPosition().x;
	const int y = event.GetPosition().y;
	int index = board_.get_matched_index( x, y );
	if (invalid_index != index)
	{
		board_.change_piece(index, Piece_Dark);
		socket_->send_setpiece(index);
		this->Refresh();
	}
	event.Skip();
}

void MyFrame::OnClick_(wxMouseEvent & event)
{
	const int x = event.GetPosition().x;
	const int y = event.GetPosition().y;
	const int index = board_.get_matched_index( x, y );
	if (invalid_index != index)
	{
		board_.change_piece(index, Piece_White);
		this->Refresh();
	}
	event.Skip();
}

void MyFrame::OnTimer(wxTimerEvent& event)
{
	//const int index = socket_->recv_setpiece();
	//board_.change_piece(index, Piece_Dark);
	this->Refresh();
}

void MyFrame::recv_thread()
{
	while(true)
	{
		boost::posix_time::seconds workTime(2);  
       
	    // Pretend to do something useful...  
		boost::this_thread::sleep(workTime);  
		const int index = socket_->recv_setpiece();
		board_.change_piece(index, Piece_White);
	}
}