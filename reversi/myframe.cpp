#include "myframe.h"
#include <boost/date_time.hpp>
#include "mysession.h"

#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <google/protobuf/text_format.h>
#include "../common/packet.pb.h"
#include "../common/common.h"

static wxColour Color_Green = wxColour( 34, 139, 34 );
static wxColour Color_Khaki = wxColour( 240, 230, 140 );
static wxColour Color_Red = wxColour( 255, 0, 0 );
static wxColour Color_Blue = wxColour( 0, 0, 255 );
static wxColour Color_Black = wxColour( 0, 0, 0 );

const int TIMER_ID = 101;
const int WORKER_EVENT = wxID_HIGHEST+1;   // this one gets sent from MyWorkerThread

static void ShowMessageBox( const wxString& message )
{
	wxMessageDialog dlg( NULL, message, "reversi", wxOK);
	dlg.ShowModal();
	dlg.Destroy();
}

static void draw_square(wxPaintDC & dc, const SquareList & list);
static void draw_piece(wxPaintDC & dc, const SquareList & list);


MyFrame::MyFrame(const wxString& title, MySession * session)
       : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(380, 400))
	   , timer_(this, TIMER_ID)
	   , session_(session)
{
	board_.init();
	this->Connect(wxEVT_PAINT, wxPaintEventHandler(MyFrame::OnPaint));
	this->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(MyFrame::OnClick));
	this->Connect(wxEVT_TIMER, wxTimerEventHandler(MyFrame::OnTimer));
	this->Connect(WORKER_EVENT, wxEVT_THREAD, wxThreadEventHandler(MyFrame::OnWorkerEvent));
	this->SetDoubleBuffered(true);
	timer_.Start(1000);
}

MyFrame::~MyFrame()
{
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
		dc.SetPen( wxPen(Color_Black, 0) );
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
		else if ( s.own == Piece_Dark_)
		{
			dc.SetPen( wxPen(Color_Khaki, 5) );
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
		board_.change_piece(index, Piece_Dark_);
		//session_->write("lalala", strlen("lalala"));
		/////////////////////////////

		//using namespace google;
		//packet::C_SetPiece pkt;
		//pkt.set_pos_index(index);
		//int body_size = pkt.ByteSize();
		//char* body_buf = new char[body_size];

		//// 버퍼에 직렬화
		//protobuf::io::ArrayOutputStream os(body_buf, body_size);
		//pkt.SerializeToZeroCopyStream(&os);

		//PacketHeader * header = new PacketHeader();
		//header->size = body_size;
		//header->code = packet::C_SET_PIECE;

		//boost::asio::const_buffer * header_buffer = new boost::asio::const_buffer(header, sizeof(PacketHeader));
		//boost::asio::const_buffer * body_buffer = new boost::asio::const_buffer(body_buf, body_size);
		////////////////////////////


		this->Refresh();
	}
	event.Skip();
}

void MyFrame::OnTimer(wxTimerEvent& event)
{
	if ( board_.update_piece() )
	{
		this->Refresh();
	}
}

void MyFrame::OnWorkerEvent(wxThreadEvent& event)
{
	this->Refresh();
}

void MyFrame::OnReceive(const char * buffer, const size_t len)
{
	wxThreadEvent thread_event(wxEVT_THREAD, WORKER_EVENT);
	//thread_event.SetInt(0);
	wxQueueEvent( this, thread_event.Clone() );

	//PacketHeader header;
	//memcpy( &header, buffer, sizeof(PacketHeader) );
	//if ( header.code == packet::S_SET_PIECE )
	//{
	//	packet::S_SetPiece pkt;
	//	using namespace google;
	//	protobuf::io::ArrayInputStream is(&buffer[sizeof(PacketHeader)], len-sizeof(PacketHeader));
	//	pkt.ParseFromZeroCopyStream(&is);

	//	board_.change_piece(pkt.index(), Piece_Dark);
	//}
}