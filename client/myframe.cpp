#include "myframe.h"
#include "mysession.h"

#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <google/protobuf/text_format.h>
#include "../common/packet.pb.h"

static wxColour Color_Green = wxColour( 34, 139, 34 );
static wxColour Color_Khaki = wxColour( 240, 230, 140 );
static wxColour Color_Red = wxColour( 255, 0, 0 );
static wxColour Color_Blue = wxColour( 0, 0, 255 );
static wxColour Color_Black = wxColour( 0, 0, 0 );
static wxColour Color_PT_White = wxColour( 250, 250, 250 );
static wxColour Color_PT_Dark = wxColour( 0, 0, 0 );

enum
{
	REVERSI_THREAD_EVENT = wxID_HIGHEST+1,
	REVERSI_LOGIN_BUTTON_ID,
	REVERSI_TIMER_ID,
};

enum
{
	TE_NextTurn = 0,
};

static void ShowMessageBox( const wxString& message )
{
	wxMessageDialog dlg( NULL, message, "reversi", wxOK);
	dlg.ShowModal();
	dlg.Destroy();
}

static void draw_square(wxPaintDC & dc, const SquareList & list);
static void draw_piece(wxPaintDC & dc, const SquareList & list);

////////////////////////////////////////////////////////////////////////

LoginDialog::LoginDialog(const wxString & title)
	: wxDialog(NULL, -1, title, wxDefaultPosition, wxSize(220, 80))
{
	wxPanel * panel = new wxPanel(this, -1);
	m_nick_text = new wxTextCtrl(panel, -1, wxT(""), wxPoint(20, 10));
	wxButton * okbutton = new wxButton(panel, REVERSI_LOGIN_BUTTON_ID, wxT("ok"), wxPoint(130, 10));
	this->Connect(REVERSI_LOGIN_BUTTON_ID, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(LoginDialog::OnOk));
	//Centre();
	ShowModal();
}

void LoginDialog::OnOk(wxCommandEvent& event)
{
	Destroy();
}
wxString LoginDialog::GetNickname() const
{
	return m_nick_text->GetValue();
}
////////////////////////////////////////////////////////////////////////

MyFrame::MyFrame(const wxString& title, MySession * session)
       : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(380, 440))
	   , timer_(this, REVERSI_TIMER_ID)
	   , session_(session)
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	board_.init();
	this->Connect(wxEVT_PAINT, wxPaintEventHandler(MyFrame::OnPaint));
	this->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(MyFrame::OnClick));
	this->Connect(wxEVT_TIMER, wxTimerEventHandler(MyFrame::OnTimer));
	this->Connect(REVERSI_THREAD_EVENT, wxEVT_THREAD, wxThreadEventHandler(MyFrame::OnWorkerEvent));
	
	this->SetDoubleBuffered(true);


	m_menubar = new wxMenuBar; 
	m_menu = new wxMenu;

	m_menu->Append(wxID_EXIT, wxT("Login"));
	m_menubar->Append(m_menu, wxT("Menu"));
	this->SetMenuBar(m_menubar);
	this->Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnLoginMenu));

	this->CreateStatusBar(2);
	this->SetStatusText(wxT("Ready"), 0);
}

MyFrame::~MyFrame()
{
	google::protobuf::ShutdownProtobufLibrary();
}

void MyFrame::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	const SquareList & square_list = board_.get_square_list();
	draw_square(dc, square_list);
	draw_piece(dc, square_list);
}

static void draw_square(wxPaintDC & dc, const SquareList & list)
{
	SquareList::const_iterator iter = list.begin();
	for ( ; iter != list.end(); ++iter)
	{
		Square s = (*iter);
		dc.SetBrush( *wxTRANSPARENT_BRUSH );
		dc.DrawRectangle(s.x - size_square, s.y - size_square, size_square, size_square);
	}
}

static void draw_piece(wxPaintDC & dc, const SquareList & list)
{
	SquareList::const_iterator iter = list.begin();
	for ( ; iter != list.end(); ++iter)
	{
		Square s = (*iter);
		if ( s.blink )
		{
			dc.SetPen( wxPen(Color_Khaki, 5) );
		}
		else
		{
			dc.SetPen( wxPen(Color_Black, 0) );
		}
		
		if ( s.own == PT_None)
		{
			continue;
		}
		else if ( s.own == PT_White)
		{
			dc.SetBrush( Color_PT_White );
			dc.DrawCircle(s.x-(size_square/2), s.y-(size_square/2), 20);
		}
		else if ( s.own == PT_Dark)
		{
			dc.SetBrush( Color_PT_Dark );
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
		/////////////////////////////
		using namespace google;
		packet::C_SetPiece pkt;
		pkt.set_pos_index(index);
		int body_size = pkt.ByteSize();
		char* body_buf = new char[body_size];

		// 버퍼에 직렬화
		protobuf::io::ArrayOutputStream os(body_buf, body_size);
		pkt.SerializeToZeroCopyStream(&os);

		session_->write( packet::C_SET_PIECE, body_buf, body_size );
		delete [] body_buf;

		this->Refresh();
	}
	event.Skip();
}

void MyFrame::OnTimer(wxTimerEvent& event)
{
	if ( board_.update_blink() )
	{
		this->Refresh();
	}
}

void MyFrame::OnWorkerEvent(wxThreadEvent& event)
{
	this->Refresh();
	timer_.Start(1000, true);
}

void MyFrame::OnLoginMenu(wxCommandEvent& event)
{
	LoginDialog * loginDialog = new LoginDialog(wxT("Login"));
	loginDialog->Show(true);
	wxString nick = loginDialog->GetNickname();

	delete loginDialog;

	using namespace google;
	packet::C_Login pkt;
	const int body_size = pkt.ByteSize();
	char* body_buf = new char[body_size];

	// 버퍼에 직렬화
	protobuf::io::ArrayOutputStream os(body_buf, body_size);
	pkt.SerializeToZeroCopyStream(&os);
	session_->write(packet::C_LOGIN, body_buf, body_size);
	delete [] body_buf;
}

void MyFrame::OnReceive(const char * buffer, const size_t len)
{
	using namespace google;
	PacketHeader header;
	memcpy( &header, buffer, sizeof(PacketHeader) );
	protobuf::io::ArrayInputStream is(&buffer[sizeof(PacketHeader)], len-sizeof(PacketHeader));
	if ( header.code == packet::S_LOGIN )
	{
		this->SetStatusText(wxT("Login success"), 0);

		packet::S_Login pkt;
		pkt.ParseFromZeroCopyStream(&is);

		board_.set_myid(pkt.id());

	}
	else if ( header.code == packet::S_START )
	{
		this->SetStatusText(wxT("Game Start"), 0);
		packet::S_Start pkt;
		pkt.ParseFromZeroCopyStream(&is);
		board_.set_mypt(pkt.your_piece());
		if (pkt.your_piece() == PT_White)
		{
			this->SetStatusText(wxT("White"), 1);
		}
		else
		{
			this->SetStatusText(wxT("Dark"), 1);
		}

		for (int i = 0; i < pkt.piece_list_size(); ++i)
		{
			int piece_type = pkt.piece_list(i);
			board_.change_piece(i, piece_type, false);
		}

		if (board_.get_mypt() == pkt.first_turn())
		{
			this->SetStatusText(wxT("My turn"), 0);
		}
		else
		{
			this->SetStatusText(wxT("Others turn"), 0);
		}
		wxThreadEvent thread_event(wxEVT_THREAD, REVERSI_THREAD_EVENT);
		wxQueueEvent( this, thread_event.Clone() );
	}
	else if ( header.code == packet::S_SET_PIECE )
	{
		packet::S_SetPiece pkt;
		pkt.ParseFromZeroCopyStream(&is);

		for (int i = 0; i < pkt.piece_list_size(); ++i)
		{
			int piece_type = pkt.piece_list(i);
			board_.change_piece(i, piece_type, true);
		}

		if (board_.get_mypt() == pkt.next_turn())
		{
			this->SetStatusText(wxT("My turn"), 0);
		}
		else
		{
			this->SetStatusText(wxT("Others turn"), 0);
		}

		wxThreadEvent thread_event(wxEVT_THREAD, REVERSI_THREAD_EVENT);
		//thread_event.SetInt(pkt.next_turn());
		wxQueueEvent( this, thread_event.Clone() );
	}
	else if ( header.code == packet::S_END )
	{
		packet::S_End pkt;
		pkt.ParseFromZeroCopyStream(&is);
		if ( pkt.winner_id() == board_.get_myid() )
		{
			ShowMessageBox(wxT("You won!"));
		}
		else
		{
			ShowMessageBox(wxT("You lost!"));
		}

		board_.init();
	}
}