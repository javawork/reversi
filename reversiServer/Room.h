#pragma once

#include "SendPacket.h"

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
		GameState_White_Trun,
		GameState_Black_Turn,
		GameState_Stop
	};

	enum Stone
	{
		Stone_None,
		Stone_White,
		Stone_Black
	};

	void Init();

	bool EnterSession(CReversySession* Session);
	void LeaveSession(CReversySession* Session);

	void StartGame();
	void StopGame();
	bool ChangeStone(int x, int y, CReversySession* Session);

	void Broadcast(CSendPacket& Packet);
	void BroadcastExceptMe(CSendPacket& Packet, CReversySession* Session);
protected:
	void CheckChangedStone(int x, int y);
	bool RecordChangedStone(int x, int y, int moveX, int moveY);
	void ChangeStone();

protected:
	RoomState m_RoomState;
	GameState m_GameState;

	CReversySession* m_Session[2];
	std::vector< int > m_HistoryChangedStone;
	int m_Board[BOARD_SIZE][BOARD_SIZE];

};

