#pragma once

#include "CoreTypes.h"

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>

#include <cstdint>
#include <cstddef>

namespace ve::network::platform
{
	inline constexpr std::uintptr_t InvalidSocketHandle = ve::core::NumericCast<std::uintptr_t>(INVALID_SOCKET);

	[[nodiscard]] constexpr SOCKET ToNativeSocket(std::uintptr_t stored_socket_handle) noexcept
	{
		return ve::core::NumericCast<SOCKET>(stored_socket_handle);
	}

	[[nodiscard]] constexpr std::uintptr_t StoreNativeSocket(SOCKET socket) noexcept
	{
		return ve::core::NumericCast<std::uintptr_t>(socket);
	}

	[[nodiscard]] constexpr int SocketAddressByteCount(std::size_t byte_count) noexcept
	{
		return ve::core::ToInt(byte_count);
	}

	[[nodiscard]] constexpr int SocketTransferByteCount(std::size_t byte_count) noexcept
	{
		return ve::core::ToInt(byte_count);
	}

	[[nodiscard]] constexpr std::size_t TransferredByteCount(int byte_count) noexcept
	{
		return ve::core::ToIndex(byte_count);
	}
}
