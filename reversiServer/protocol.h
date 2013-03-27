#pragma once

struct PROTOCOL_ID
{
	enum 
	{
		C_Login = 1,
		S_Login,
		S_StartGame,
		C_SetPiece,
		S_SetPiece
	};
};

