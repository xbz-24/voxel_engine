namespace
{
	class PayloadWriter
	{
	public:
		template <typename Value>
		void Write(const Value& value)
		{
			static_assert(std::is_trivially_copyable_v<Value>);
			const auto* firstSerializedByte = reinterpret_cast<const std::byte*>(&value);
			_payloadBytes.insert(_payloadBytes.end(), firstSerializedByte, firstSerializedByte + sizeof(Value));
		}

		void WriteBytes(std::span<const std::byte> sourceBytes)
		{
			_payloadBytes.insert(_payloadBytes.end(), sourceBytes.begin(), sourceBytes.end());
		}

		[[nodiscard]] ve::network::ByteBuffer Finish() &&
		{
			return std::move(_payloadBytes);
		}

	private:
		ve::network::ByteBuffer _payloadBytes;
	};

	class PayloadReader
	{
	public:
		explicit PayloadReader(std::span<const std::byte> serializedPayloadBytes) noexcept
			: _serializedPayloadBytes(serializedPayloadBytes)
		{
		}

		template <typename Value>
		bool Read(Value& output)
		{
			static_assert(std::is_trivially_copyable_v<Value>);
			if (_readOffset > _serializedPayloadBytes.size()) return false;
			if (_serializedPayloadBytes.size() - _readOffset < sizeof(Value)) return false;
			std::memcpy(&output, _serializedPayloadBytes.data() + _readOffset, sizeof(Value));
			_readOffset += sizeof(Value);
			return true;
		}

		[[nodiscard]] bool IsFinished() const noexcept
		{
			return _readOffset == _serializedPayloadBytes.size();
		}

	private:
		std::span<const std::byte> _serializedPayloadBytes;
		std::size_t _readOffset = 0;
	};
}
