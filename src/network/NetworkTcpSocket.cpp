#include "NetworkTcpSocket.h"

#include "NetworkSocketPlatform.h"

namespace ve::network
{
	TcpSocket::TcpSocket() noexcept : _nativeSocketHandle(platform::InvalidSocketHandle) {}
	TcpSocket::TcpSocket(std::uintptr_t nativeSocketHandle) noexcept : _nativeSocketHandle(nativeSocketHandle) {}
	TcpSocket::~TcpSocket() { Close(); }

	TcpSocket::TcpSocket(TcpSocket&& other) noexcept : _nativeSocketHandle(other._nativeSocketHandle)
	{
		other._nativeSocketHandle = platform::InvalidSocketHandle;
	}

	TcpSocket& TcpSocket::operator=(TcpSocket&& other) noexcept
	{
		if (this != &other)
		{
			Close();
			_nativeSocketHandle = other._nativeSocketHandle;
			other._nativeSocketHandle = platform::InvalidSocketHandle;
		}
		return *this;
	}

	void TcpSocket::Close() noexcept
	{
		if (IsOpen())
		{
			closesocket(platform::ToNativeSocket(_nativeSocketHandle));
			_nativeSocketHandle = platform::InvalidSocketHandle;
		}
	}

	bool TcpSocket::IsOpen() const noexcept
	{
		return _nativeSocketHandle != platform::InvalidSocketHandle;
	}
}
