#pragma once

#include "SendPacket.h"
#include "common/common.h"

class CReversySession;
class CReversyServer;

class CRoom
{
public:
	CRoom(void);
	~CRoom(void);

	enum RoomState
	{
		RoomState_Wait,
		RoomState_Playing,
		RoomState_Stop
	};

	enum GameState
	{
		GameState_Black_Turn,
		GameState_White_Trun,
		GameState_Stop
	};

	void Init();

	int  EnterSession(CReversySession* Session);
	void LeaveSession(CReversySession* Session);

	void TryStartGame();
	void StopGame();
	bool SetPiece(int x, int y, CReversySession* Session);

	void Broadcast(CSendPacket& Packet);
	void BroadcastExceptMe(CSendPacket& Packet, CReversySession* Session);

protected:
	void CheckChangedPiece(int x, int y, PieceType iPieceType, std::vector< int >& outArr);
	void RecordChangedPiece(int x, int y, int moveX, int moveY, PieceType iPieceType, std::vector< int >& outArr);
	void ChangePiece(PieceType iPieceType, std::vector< int >& outArr);
	bool ExistPutableSpace(PieceType iPieceType);

	void SetNextTurn();
	void SetGameEnd();

protected:
	RoomState m_RoomState;
	GameState m_GameState;

	CReversySession* m_Session[2];
	PieceType m_Board[BOARD_SIZE][BOARD_SIZE];

};

