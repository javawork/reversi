#pragma once

struct PacketHeader
{
	short size;
	short code;
};

enum PieceType 
{
	PT_None,
	PT_White,
	PT_Dark,
};