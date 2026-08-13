#pragma once

#include "NetworkProtocol.h"

namespace ve::network::detail
{
	bool IsKnownMessageType(NetworkMessageType messageType) noexcept;
	std::uint32_t CalculatePayloadChecksum(std::span<const std::byte> payloadBytes) noexcept;
}
