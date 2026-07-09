namespace
{
	class PayloadWriter
	{
	public:
		template <typename Value>
		void Write(const Value& value)
		{
			ve::network::AppendSerializedValue(_payloadBytes, value);
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
			return ve::network::ReadSerializedValue(_serializedPayloadBytes, _readOffset, output);
		}

		[[nodiscard]] std::optional<std::span<const std::byte>> ReadBytes(std::size_t byteCount)
		{
			return ve::network::ReadByteSpan(_serializedPayloadBytes, _readOffset, byteCount);
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
