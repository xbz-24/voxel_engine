#include "NetworkSocketLibrary.h"

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>

namespace ve::network
{
	SocketLibrary::SocketLibrary()
	{
		WSADATA socketStartupData{};
		_isStarted = WSAStartup(MAKEWORD(2, 2), &socketStartupData) == 0;
	}

	SocketLibrary::~SocketLibrary()
	{
		if (_isStarted)
		{
			WSACleanup();
		}
	}

	bool SocketLibrary::IsAvailable() const noexcept
	{
		return _isStarted;
	}
}
