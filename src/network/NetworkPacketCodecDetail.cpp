#include "NetworkPacketCodecDetail.h"

namespace ve::network::detail
{
	bool IsKnownMessageType(NetworkMessageType messageType) noexcept
	{
		switch (messageType)
		{
		case NetworkMessageType::ClientHello:
		case NetworkMessageType::ServerWelcome:
		case NetworkMessageType::PlayerSnapshot:
		case NetworkMessageType::BlockMutation:
		case NetworkMessageType::Ping:
		case NetworkMessageType::Pong:
		case NetworkMessageType::Disconnect:
			return true;
		}
		return false;
	}

	std::uint32_t CalculatePayloadChecksum(std::span<const std::byte> payloadBytes) noexcept
	{
		constexpr std::uint32_t offsetBasis = 2166136261U;
		constexpr std::uint32_t prime = 16777619U;
		std::uint32_t checksum = offsetBasis;
		for (const std::byte payloadByte : payloadBytes)
		{
			checksum ^= std::to_integer<std::uint32_t>(payloadByte);
			checksum *= prime;
		}
		return checksum;
	}
}
