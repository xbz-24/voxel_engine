namespace
{
	class RecordingComputeDispatcher final : public ve::rendering::ComputeDispatcher
	{
	public:
		explicit RecordingComputeDispatcher(bool supported) noexcept
			: supported_(supported)
		{
		}

		[[nodiscard]] bool IsSupported() const noexcept override
		{
			return supported_;
		}

		void Dispatch(
			ve::rendering::ComputeProgramHandle program,
			ve::rendering::ComputeDispatchSize size) const override
		{
			lastProgram = program;
			lastDispatchSize = size;
			dispatchCount++;
		}

		bool supported_ = false;
		mutable ve::rendering::ComputeProgramHandle lastProgram{};
		mutable ve::rendering::ComputeDispatchSize lastDispatchSize{};
		mutable int dispatchCount = 0;
	};
}

TEST_CASE("compute terrain generator reports whether dispatch work was submitted")
{
	RecordingComputeDispatcher supportedDispatcher(true);
	ve::world::generation::ComputeTerrainGenerator generator(
		ve::rendering::ComputeProgramHandle{ 7U },
		&supportedDispatcher);

	CHECK(generator.Dispatch(2U, 3U, 4U));
	CHECK(supportedDispatcher.dispatchCount == 1);
	CHECK(supportedDispatcher.lastProgram.value == 7U);
	CHECK(supportedDispatcher.lastDispatchSize.groups_x == 2U);
	CHECK(supportedDispatcher.lastDispatchSize.groups_y == 3U);
	CHECK(supportedDispatcher.lastDispatchSize.groups_z == 4U);
	CHECK(!generator.Dispatch(0U, 1U, 1U));
	CHECK(supportedDispatcher.dispatchCount == 1);
}

TEST_CASE("compute terrain generator rejects missing backend or program")
{
	RecordingComputeDispatcher unsupportedDispatcher(false);
	ve::world::generation::ComputeTerrainGenerator missingProgram(
		ve::rendering::ComputeProgramHandle{},
		&unsupportedDispatcher);
	ve::world::generation::ComputeTerrainGenerator missingDispatcher(
		ve::rendering::ComputeProgramHandle{ 3U },
		nullptr);

	CHECK(!missingProgram.Dispatch(1U, 1U, 1U));
	CHECK(!missingDispatcher.Dispatch(1U, 1U, 1U));
	CHECK(unsupportedDispatcher.dispatchCount == 0);
}
