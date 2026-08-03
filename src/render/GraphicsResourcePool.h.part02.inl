			if (free_slots_.empty())
			{
				slots_.push_back(Slot{});
				return ve::core::ToU32(slots_.size() - 1U);
			}
			const std::uint32_t slot_index = free_slots_.back();
			free_slots_.pop_back();
			return slot_index;
		}

		ve::core::DynamicArray<Slot> slots_;
		ve::core::DynamicArray<std::uint32_t> free_slots_;
		ve::core::Index live_count_ = 0;
	};
}
