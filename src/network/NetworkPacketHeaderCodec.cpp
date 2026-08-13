#include "NetworkProtocol.h"

#include "CoreTypes.h"
#include "NetworkByteCodec.h"

#include <cstddef>
#include <utility>

namespace
{
	constexpr std::uint32_t PayloadChecksumOffsetBasis = 2166136261U;
	constexpr std::uint32_t PayloadChecksumPrime = 16777619U;
	constexpr std::uint32_t SerializedPacketHeaderByteCount =
		sizeof(std::uint32_t) + sizeof(std::uint16_t) + sizeof(ve::network::NetworkMessageType) +
		sizeof(std::uint32_t) + sizeof(std::uint32_t) + sizeof(std::uint32_t);
	static_assert(ve::network::PacketHeaderByteCount == SerializedPacketHeaderByteCount);

	bool IsKnownMessageType(ve::network::NetworkMessageType messageType)
	{
		switch (messageType)
		{
		case ve::network::NetworkMessageType::ClientHello:
		case ve::network::NetworkMessageType::ServerWelcome:
		case ve::network::NetworkMessageType::PlayerSnapshot:
		case ve::network::NetworkMessageType::BlockMutation:
		case ve::network::NetworkMessageType::Ping:
		case ve::network::NetworkMessageType::Pong:
		case ve::network::NetworkMessageType::Disconnect:
			return true;
		}
		return false;
	}

	std::uint32_t CalculatePayloadChecksum(std::span<const std::byte> payloadBytes)
	{
		std::uint32_t payloadChecksum = PayloadChecksumOffsetBasis;
		for (const std::byte payloadByte : payloadBytes)
		{
			payloadChecksum ^= std::to_integer<std::uint32_t>(payloadByte);
			payloadChecksum *= PayloadChecksumPrime;
		}
		return payloadChecksum;
	}

	template <typename Value>
	void AppendHeaderField(ve::network::ByteBuffer& packetBytes, const Value& value)
	{
		ve::network::AppendSerializedValue(packetBytes, value);
	}

	template <typename Value>
	[[nodiscard]] bool ReadHeaderField(
		std::span<const std::byte> headerBytes,
		std::size_t& headerByteOffset,
		Value& output)
	{
		return ve::network::ReadSerializedValue(headerBytes, headerByteOffset, output);
	}
}
namespace ve::network
{
	ByteBuffer BuildPacket(
		NetworkMessageType messageType,
		std::span<const std::byte> payloadBytes,
		std::uint32_t sequenceNumber)
	{
		if (payloadBytes.size() > ve::core::ToIndex(MaxPacketPayloadByteCount)) return {};
		if (!IsKnownMessageType(messageType)) return {};
		ByteBuffer packetBytes;
		const PacketHeader packetHeader{
			ProtocolMagic,
			ProtocolVersion,
			messageType,
			sequenceNumber,
			ve::core::ToU32(payloadBytes.size()),
			CalculatePayloadChecksum(payloadBytes)
		};
		packetBytes.reserve(PacketHeaderByteCount + payloadBytes.size());
		AppendHeaderField(packetBytes, packetHeader.magic);
		AppendHeaderField(packetBytes, packetHeader.version);
		AppendHeaderField(packetBytes, packetHeader.messageType);
		AppendHeaderField(packetBytes, packetHeader.sequenceNumber);
		AppendHeaderField(packetBytes, packetHeader.payloadByteCount);
		AppendHeaderField(packetBytes, packetHeader.payloadChecksum);
		packetBytes.insert(packetBytes.end(), payloadBytes.begin(), payloadBytes.end());
		return packetBytes;
	}

	std::optional<PacketHeader> TryParsePacketHeader(std::span<const std::byte> headerBytes)
	{
		if (headerBytes.size() != PacketHeaderByteCount) return std::nullopt;
		PacketHeader packetHeader{};
		std::size_t headerByteOffset = 0;
		if (!ReadHeaderField(headerBytes, headerByteOffset, packetHeader.magic)) return std::nullopt;
		if (!ReadHeaderField(headerBytes, headerByteOffset, packetHeader.version)) return std::nullopt;
		if (!ReadHeaderField(headerBytes, headerByteOffset, packetHeader.messageType)) return std::nullopt;
		if (!ReadHeaderField(headerBytes, headerByteOffset, packetHeader.sequenceNumber)) return std::nullopt;
		if (!ReadHeaderField(headerBytes, headerByteOffset, packetHeader.payloadByteCount)) return std::nullopt;
		if (!ReadHeaderField(headerBytes, headerByteOffset, packetHeader.payloadChecksum)) return std::nullopt;
		if (packetHeader.magic != ProtocolMagic || packetHeader.version != ProtocolVersion) return std::nullopt;
		if (!IsKnownMessageType(packetHeader.messageType)) return std::nullopt;
		if (packetHeader.payloadByteCount > MaxPacketPayloadByteCount) return std::nullopt;
		return packetHeader;
	}

	std::optional<NetworkMessage> TryParsePacket(std::span<const std::byte> packetBytes)
	{
		if (packetBytes.size() < PacketHeaderByteCount) return std::nullopt;
		const std::span<const std::byte> packetHeaderBytes(packetBytes.data(), PacketHeaderByteCount);
		const std::optional<PacketHeader> packetHeader = TryParsePacketHeader(packetHeaderBytes);
		if (!packetHeader.has_value()) return std::nullopt;
		const std::size_t payloadByteCount = packetHeader->payloadByteCount;
		if (packetBytes.size() - PacketHeaderByteCount != payloadByteCount) return std::nullopt;
		const std::span<const std::byte> payloadBytes(packetBytes.data() + PacketHeaderByteCount, payloadByteCount);
		if (CalculatePayloadChecksum(payloadBytes) != packetHeader->payloadChecksum) return std::nullopt;
		ByteBuffer messagePayloadBytes(payloadBytes.begin(), payloadBytes.end());
		return NetworkMessage{
			packetHeader->messageType,
			std::move(messagePayloadBytes),
			packetHeader->sequenceNumber
		};
	}
}
