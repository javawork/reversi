#include "StdAfx.h"
#include "Room.h"
#include "ReversySession.h"
#include "Common/Packet.pb.h"

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
	memset(m_Board, PT_None, sizeof(m_Board));
}

int CRoom::EnterSession(CReversySession* Session)
{
	if( RoomState_Wait != m_RoomState )
		return -1;

	int iRet = -1;

	if( nullptr == m_Session[0] )
	{
		m_Session[0] = Session;
		iRet = 0;
	}
	else if( nullptr == m_Session[1] )
	{
		m_Session[1] = Session;
		iRet = 1;
	}
	
	return iRet;
}

void CRoom::LeaveSession(CReversySession* pSession)
{
	if( pSession == m_Session[0] )
	{
		m_Session[0] = nullptr;
	}
	else if( pSession == m_Session[1] )
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

void CRoom::TryStartGame()
{
	if( !m_Session[0] || !m_Session[1] )
		return;

	m_RoomState = RoomState_Playing;
	m_GameState = GameState_White_Trun;
	memset(m_Board, PT_None, sizeof(m_Board));

	int iCenter = BOARD_SIZE/2;

	m_Board[iCenter-1][iCenter-1] = PT_White;
	m_Board[iCenter][iCenter-1] = PT_Dark;
	m_Board[iCenter-1][iCenter] = PT_Dark;
	m_Board[iCenter][iCenter] = PT_White;

	::packet::S_Start StartInfo;
	StartInfo.set_your_piece(PT_Dark); 
	for( int i = 0 ; i < BOARD_SIZE*BOARD_SIZE ; ++i )
	{
		StartInfo.add_piece_list( ((int*)m_Board)[i] );
	}
	StartInfo.set_first_turn(PT_Dark);

	CSendPacket Packet;
	Packet.SetID( packet::S_START );
	Packet.PackProtoBuf( &StartInfo );
	m_Session[0]->Send(Packet);

	StartInfo.set_your_piece(PT_White);
	Packet.Init();
	Packet.SetID( packet::S_START );
	Packet.PackProtoBuf( &StartInfo );
	m_Session[1]->Send(Packet);

	m_RoomState = RoomState_Playing;
	m_GameState = GameState_Black_Turn;		//First turn is black piece
}

void CRoom::StopGame()
{
	m_RoomState = RoomState_Stop;
	m_GameState = GameState_Stop;
	memset(m_Board, PT_None, sizeof(m_Board));
}

bool CRoom::SetPiece(int x, int y, CReversySession* pSession)
{
	if( GameState::GameState_Stop == m_GameState )
		return false;

	if( nullptr == pSession || pSession != m_Session[m_GameState] )
		return false;

	PieceType iPiece = (GameState::GameState_Black_Turn == m_GameState) ? PT_Dark : PT_White;


	if( x < 0 || y < 0 || x >= BOARD_SIZE || y >= BOARD_SIZE )
		return false;

	std::vector< int > History;

	CheckChangedPiece(x, y, iPiece, History);

	if( History.empty() )
		return false;

	History.push_back( (y*BOARD_SIZE)+x );
	//change pieces
	ChangePiece(iPiece, History);

	SetNextTurn();

	//Send SetPiece
	::packet::S_SetPiece ProtoBuf;
	ProtoBuf.set_piece( iPiece );
	ProtoBuf.set_pos_index( (y*BOARD_SIZE)+x );
	
	for( int i = 0 ; i < BOARD_SIZE*BOARD_SIZE ; ++i )
	{
		ProtoBuf.add_piece_list( ((int*)m_Board)[i] );
	}

	switch( m_GameState )
	{
	case GameState::GameState_Black_Turn:	ProtoBuf.set_next_turn(PieceType::PT_Dark);		break;
	case GameState::GameState_White_Trun:	ProtoBuf.set_next_turn(PieceType::PT_White);	break;
	default:								ProtoBuf.set_next_turn(PieceType::PT_None);		break;
	}

	CSendPacket Packet;
	Packet.SetID( ::packet::MessageType::S_SET_PIECE );
	Packet.PackProtoBuf( &ProtoBuf );

	Broadcast(Packet);

	if( GameState::GameState_Stop == m_GameState )
		SetGameEnd();

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

void CRoom::CheckChangedPiece(int x, int y, PieceType iPieceType, std::vector< int >& outArr)
{
	const int DIRECTION[8][2] = { {+1, +1}, {+1, 0}, {+1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, +1}, {0, +1} };

	for( int i = 0 ; i < 8 ; ++i )
	{
		RecordChangedPiece(x, y, DIRECTION[i][0], DIRECTION[i][1], iPieceType, outArr);
	}

}

bool CRoom::RecordChangedPiece(int x, int y, int moveX, int moveY, PieceType iPieceType, std::vector< int >& outArr)
{
	x += moveX;
	y += moveY;

	if( x < 0 || y < 0 || x >= BOARD_SIZE || y >= BOARD_SIZE )
		return false;

	if( PieceType::PT_None == m_Board[y][x] )
		return false;

	if( iPieceType == m_Board[y][x] )
		return true;

	bool bRet = RecordChangedPiece(x, y, moveX, moveY, iPieceType, outArr);
	if( true == bRet )
	{	
		outArr.push_back( (y*BOARD_SIZE)+x );
	}

	return bRet;
}

void CRoom::ChangePiece(PieceType iPieceType, std::vector< int >& outArr)
{
	auto fnChangePiece = [&](int& iPos)
	{
		m_Board[iPos/BOARD_SIZE][iPos%BOARD_SIZE] = iPieceType;
	};

	std::for_each( outArr.begin(), outArr.end(), fnChangePiece);
}

bool CRoom::ExistPutableSpace(PieceType iPieceType)
{
	std::vector< int > History(BOARD_SIZE*BOARD_SIZE);

	for( int i = 0 ; i < BOARD_SIZE ; ++i )
	{
		for( int j = 0 ; j < BOARD_SIZE ; ++j )
		{
			if( PieceType::PT_None == m_Board[i][j] )
			{
				CheckChangedPiece(i, j, iPieceType, History);

				if( !History.empty() )
					return true;
			}
		}
	}

	return false;
}

void CRoom::SetNextTurn()
{
	if( GameState::GameState_Black_Turn == m_GameState )
	{
		if( true == ExistPutableSpace( PieceType::PT_White ) )
		{
			m_GameState = GameState::GameState_White_Trun;
			return;
		}
		else if( true == ExistPutableSpace( PieceType::PT_Dark ) )
		{
			m_GameState = GameState::GameState_Black_Turn;
			return;
		}
	}
	else if( GameState::GameState_White_Trun == m_GameState )
	{
		if( true == ExistPutableSpace( PieceType::PT_Dark ) )
		{
			m_GameState = GameState::GameState_Black_Turn;
			return;
		}
		else if( true == ExistPutableSpace( PieceType::PT_White ) )
		{
			m_GameState = GameState::GameState_White_Trun;
			return;
		}
	}
	m_GameState = GameState::GameState_Stop;
}

void CRoom::SetGameEnd()
{
	int iCount[PieceType::PT_Dark+1]; 

	for( int i = 0 ; i < BOARD_SIZE*BOARD_SIZE ; ++i )
	{
		iCount[ ((int*)m_Board)[i] ]++;
	}

	::packet::S_End ProtoBuf;
	
	if( iCount[PieceType::PT_White] > iCount[PieceType::PT_Dark] )
	{
		ProtoBuf.set_winner_id(1);
	}
	else if( iCount[PieceType::PT_White] < iCount[PieceType::PT_Dark] )
	{
		ProtoBuf.set_winner_id(0);
	}
	else
	{
		ProtoBuf.set_winner_id(2);
	}
	
	CSendPacket Packet;
	Packet.SetID(::packet::MessageType::S_END);
	Packet.PackProtoBuf( &ProtoBuf );
	Broadcast( Packet );
}



