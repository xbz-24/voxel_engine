namespace
{
	class PayloadWriter
	{
	public:
		template <typename Value>
		void Write(const Value& value)
		{
			static_assert(std::is_trivially_copyable_v<Value>);
			const auto* first_byte = reinterpret_cast<const std::byte*>(&value);
			bytes_.insert(bytes_.end(), first_byte, first_byte + sizeof(Value));
		}

		void WriteBytes(std::span<const std::byte> bytes)
		{
			bytes_.insert(bytes_.end(), bytes.begin(), bytes.end());
		}

		[[nodiscard]] ve::network::ByteBuffer Finish() &&
		{
			return std::move(bytes_);
		}

	private:
		ve::network::ByteBuffer bytes_;
	};

	class PayloadReader
	{
	public:
		explicit PayloadReader(std::span<const std::byte> bytes) noexcept
			: bytes_(bytes)
		{
		}

		template <typename Value>
		bool Read(Value& output)
		{
			static_assert(std::is_trivially_copyable_v<Value>);
			if (offset_ > bytes_.size()) return false;
			if (bytes_.size() - offset_ < sizeof(Value)) return false;
			std::memcpy(&output, bytes_.data() + offset_, sizeof(Value));
			offset_ += sizeof(Value);
			return true;
		}

		[[nodiscard]] bool IsFinished() const noexcept
		{
			return offset_ == bytes_.size();
		}

	private:
		std::span<const std::byte> bytes_;
		std::size_t offset_ = 0;
	};
}
