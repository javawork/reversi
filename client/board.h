#pragma once

#include <vector>
#include "../common/common.h"

struct Square
{
	int x;
	int y;
	int index;
	int own;
	bool blink;
};

const int max_square = 6;
const int size_square = 60;
const int invalid_index = -1;

typedef std::vector<Square> SquareList;

class Board
{
public:
	Board()
	{}
	~Board()
	{}

	void init()
	{
		int index = 0;
		for (int y=0;y<max_square;++y)
		{
			for (int x=0;x<max_square;++x)
			{
				Square s;
				s.x = (x+1)*size_square;
				s.y = (y+1)*size_square;
				s.index = index;
				s.own = PT_None;
				s.blink = false;
				square_list_.push_back(s);
				++index;
			}
		}
	};

	const SquareList & get_square_list() const
	{
		return square_list_;
	}

	int get_matched_index(const int x, const int y)
	{
		SquareList::const_iterator iter = square_list_.begin();
		int prev_x = 0;
		int prev_y = 0;
		for ( ; iter != square_list_.end(); ++iter)
		{
			Square s = (*iter);
			if ( x >= prev_x && x <= s.x &&
				y >= prev_y && y <= s.y )
			{
				return s.index;
			}

			if (s.index % max_square == max_square-1)
			{
				prev_x = 0;
				prev_y = s.y;
			}
			else
			{
				prev_x = s.x;
			}
		}
		return invalid_index;
	}

	void change_piece(const int index, const int piece, const bool blink)
	{
		SquareList::iterator iter = square_list_.begin();
		for ( ; iter != square_list_.end(); ++iter)
		{
			if (iter->index == index)
			{
				if ( iter->own == piece)
					return;

				iter->own = piece;
				iter->blink = blink;
				return;
			}
		}
	}

	bool update_blink()
	{
		bool updated = false;
		SquareList::iterator iter = square_list_.begin();
		for ( ; iter != square_list_.end(); ++iter)
		{
			if (iter->blink)
			{
				iter->blink = false;
				updated = true;
			}
		}
		return updated;
	}
	
	void set_myid(const int id)
	{
		myid_ = id;
	}

	int get_myid()
	{
		return myid_;
	}

	void set_mypt(const int pt)
	{
		mypt_ = pt;
	}

	int get_mypt()
	{
		return mypt_;
	}

private:
	SquareList square_list_;
	int myid_;
	int mypt_;

};
