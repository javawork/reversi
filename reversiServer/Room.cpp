#include "StdAfx.h"
#include "Room.h"
#include "ReversySession.h"
#include "protocol.h"

CRoom::CRoom(void)
{
	Init();
}


CRoom::~CRoom(void)
{
}

void CRoom::Init()
{
	m_RoomState = RoomState_Wait;
	m_GameState = GameState_Stop;

	memset(m_Session, 0, sizeof(m_Session));
	m_HistoryChangedStone.clear();
	memset(m_Board, Stone_None, sizeof(m_Board));
}

bool CRoom::EnterSession(CReversySession* Session)
{
	if( RoomState_Wait != m_RoomState )
		return false;

	if( nullptr == m_Session[0] )
	{
		m_Session[0] = Session;
	}
	else if( nullptr == m_Session[1] )
	{
		m_Session[1] = Session;
	}
	else
	{
		return false;
	}

	if( m_Session[0] && m_Session[1] )
	{
		StartGame();
	}

	return true;
}

void CRoom::LeaveSession(CReversySession* Session)
{
	if( Session == m_Session[0] )
	{
		m_Session[0] = nullptr;
	}
	else if( Session == m_Session[1] )
	{
		m_Session[1] = nullptr;
	}
	else
	{
		return;
	}

	StopGame();
	m_RoomState = RoomState_Wait;
}

void CRoom::StartGame()
{
	if( !m_Session[0] && !m_Session[1] )
		return;

	m_RoomState = RoomState_Playing;
	m_GameState = GameState_White_Trun;
	m_HistoryChangedStone.clear();
	memset(m_Board, Stone_None, sizeof(m_Board));

	int iCenter = BOARD_SIZE/2;

	m_Board[iCenter-1][iCenter-1] = Stone_White;
	m_Board[iCenter][iCenter-1] = Stone_Black;
	m_Board[iCenter-1][iCenter] = Stone_Black;
	m_Board[iCenter][iCenter] = Stone_White;

	CSendPacket Packet;
	Packet.SetID( PROTOCOL_ID::S_StartGame );
	Broadcast(Packet);
}

void CRoom::StopGame()
{
	m_RoomState = RoomState_Stop;
	m_GameState = GameState_Stop;
	m_HistoryChangedStone.clear();
	memset(m_Board, Stone_None, sizeof(m_Board));
}

bool CRoom::ChangeStone(int x, int y, CReversySession* Session)
{
	//if( m_RoomState )

	if( Session != m_Session[m_GameState] )
		return false;

	if( x < 0 || y < 0 || x >= BOARD_SIZE || y >= BOARD_SIZE )
		return false;

	return true;
}

void CRoom::Broadcast(CSendPacket& Packet)
{
	for( int i =  0 ; i < 2 ; i++)
	{
		if( m_Session[i] )
		{
			m_Session[i]->Send(Packet);
		}
	}
}

void CRoom::BroadcastExceptMe(CSendPacket& Packet, CReversySession* Session)
{
	for( int i =  0 ; i < 2 ; i++)
	{
		if( m_Session[i] && Session != m_Session[i] )
		{
			m_Session[i]->Send(Packet);
		}
	}
}

void CRoom::CheckChangedStone(int x, int y)
{
}

bool CRoom::RecordChangedStone(int x, int y, int moveX, int moveY)
{
	return true;
}

void CRoom::ChangeStone()
{
}



