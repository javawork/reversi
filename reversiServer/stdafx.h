// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <iostream>


// TODO: reference additional headers your program requires here
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <vector>

#define Protocol_Header_Total_Size		4			
#define Protocol_Position_Of_Size		0
#define Protocol_Position_Of_Id			2

#define MAX_PACKET_SIZE				10240

#define BOARD_SIZE						6