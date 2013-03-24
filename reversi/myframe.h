#pragma once

#include <wx/wx.h>
#include "board.h"
#include <boost/thread.hpp>

class MySocket;

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title);

	void OnPaint(wxPaintEvent & event);
	void OnClick(wxMouseEvent & event);
	void OnClick_(wxMouseEvent & event);
	void OnTimer(wxTimerEvent& event);
	void recv_thread();

protected:
	void draw_line(wxPaintDC & dc, const SquareList & list);
	void draw_piece(wxPaintDC & dc, const SquareList & list);

private:
	Board board_;
	wxTimer timer_;
	MySocket * socket_;
	boost::thread    m_Thread;  
};

