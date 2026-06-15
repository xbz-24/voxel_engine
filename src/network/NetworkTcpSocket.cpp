#include "NetworkTcpSocket.h"

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>

namespace
{
	constexpr std::uintptr_t InvalidSocketHandle = static_cast<std::uintptr_t>(INVALID_SOCKET);

	/// Converts the stored integer handle back to a WinSock socket.
	SOCKET ToSocket(std::uintptr_t nativeSocketHandle) noexcept
	{
		return static_cast<SOCKET>(nativeSocketHandle);
	}
}

namespace ve::network
{
	TcpSocket::TcpSocket() noexcept : _nativeSocketHandle(InvalidSocketHandle) {}
	TcpSocket::TcpSocket(std::uintptr_t nativeSocketHandle) noexcept : _nativeSocketHandle(nativeSocketHandle) {}
	TcpSocket::~TcpSocket() { Close(); }

	TcpSocket::TcpSocket(TcpSocket&& other) noexcept : _nativeSocketHandle(other._nativeSocketHandle)
	{
		other._nativeSocketHandle = InvalidSocketHandle;
	}

	TcpSocket& TcpSocket::operator=(TcpSocket&& other) noexcept
	{
		if (this != &other)
		{
			Close();
			_nativeSocketHandle = other._nativeSocketHandle;
			other._nativeSocketHandle = InvalidSocketHandle;
		}
		return *this;
	}

	void TcpSocket::Close() noexcept
	{
		if (IsOpen())
		{
			closesocket(ToSocket(_nativeSocketHandle));
			_nativeSocketHandle = InvalidSocketHandle;
		}
	}

	bool TcpSocket::IsOpen() const noexcept
	{
		return _nativeSocketHandle != InvalidSocketHandle;
	}
}
