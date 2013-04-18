#pragma once

#include <wx/wx.h>
#include "board.h"
#include "packetlistener.h"

class MySession;

class MyFrame : public wxFrame, public PacketListener
{
public:
    MyFrame(const wxString& title, MySession * session);
	~MyFrame();

	void OnPaint(wxPaintEvent & event);
	void OnClick(wxMouseEvent & event);
	void OnTimer(wxTimerEvent& event);
	void OnWorkerEvent(wxThreadEvent& event);

	virtual void OnReceive(const char * buffer, const size_t len);

private:
	Board board_;
	wxTimer timer_;
	MySession * session_;
};

