#pragma once

struct PacketHeader
{
	short size;
	short code;
};

enum PieceType {
	PT_None = 0,
	PT_O = 1,
	PT_X = 2,
};