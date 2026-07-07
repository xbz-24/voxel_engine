namespace
{
	void PressKey(ve::input::InputSnapshot& input, ve::input::Key key) noexcept
	{
		input.keys[static_cast<std::size_t>(key)] = true;
	}

	void ReleaseKey(ve::input::InputSnapshot& input, ve::input::Key key) noexcept
	{
		input.keys[static_cast<std::size_t>(key)] = false;
	}

	void PressMouseButton(ve::input::InputSnapshot& input, ve::input::MouseButton button) noexcept
	{
		input.mouse_buttons[static_cast<std::size_t>(button)] = true;
	}
}

TEST_CASE("gameplay input exposes default action bindings")
{
	const std::span<const ve::gameplay::GameplayInputBinding> bindings =
		ve::gameplay::DefaultGameplayInputBindings();

	CHECK(bindings.size() >= 12);
	CHECK(std::ranges::any_of(bindings, [](const ve::gameplay::GameplayInputBinding& binding) {
		return binding.action == ve::gameplay::GameplayAction::MoveForward &&
			std::get_if<ve::input::Key>(&binding.control) != nullptr &&
			*std::get_if<ve::input::Key>(&binding.control) == ve::input::Key::W;
	}));
	CHECK(std::ranges::any_of(bindings, [](const ve::gameplay::GameplayInputBinding& binding) {
		return binding.action == ve::gameplay::GameplayAction::BreakBlock &&
			std::get_if<ve::input::MouseButton>(&binding.control) != nullptr &&
			*std::get_if<ve::input::MouseButton>(&binding.control) == ve::input::MouseButton::Left;
	}));
}

TEST_CASE("gameplay input reads key and mouse backed actions")
{
	ve::input::InputSnapshot input{};

	PressKey(input, ve::input::Key::W);
	PressMouseButton(input, ve::input::MouseButton::Left);

	CHECK(ve::gameplay::IsGameplayActionPressed(input, ve::gameplay::GameplayAction::MoveForward));
	CHECK(ve::gameplay::IsGameplayActionPressed(input, ve::gameplay::GameplayAction::BreakBlock));
	CHECK_FALSE(ve::gameplay::IsGameplayActionPressed(input, ve::gameplay::GameplayAction::MoveBackward));
}

TEST_CASE("gameplay input consumes actions as rising edges")
{
	ve::input::InputSnapshot input{};
	bool was_pressed = false;

	CHECK_FALSE(ve::gameplay::ConsumeGameplayAction(
		input,
		ve::gameplay::GameplayAction::ToggleFly,
		was_pressed));

	PressKey(input, ve::input::Key::F);
	CHECK(ve::gameplay::ConsumeGameplayAction(
		input,
		ve::gameplay::GameplayAction::ToggleFly,
		was_pressed));
	CHECK_FALSE(ve::gameplay::ConsumeGameplayAction(
		input,
		ve::gameplay::GameplayAction::ToggleFly,
		was_pressed));

	ReleaseKey(input, ve::input::Key::F);
	CHECK_FALSE(ve::gameplay::ConsumeGameplayAction(
		input,
		ve::gameplay::GameplayAction::ToggleFly,
		was_pressed));

	PressKey(input, ve::input::Key::F);
	CHECK(ve::gameplay::ConsumeGameplayAction(
		input,
		ve::gameplay::GameplayAction::ToggleFly,
		was_pressed));
}
