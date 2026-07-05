TEST_CASE("runtime input router applies mouse look deltas to a bound camera")
{
	Camera camera{};
	bool ui_captures_mouse = false;
	ve::gameplay::MouseLookSettings settings{};
	settings.sensitivity_degrees_per_pixel = 0.5f;
	ve::engine::RuntimeInputRouter input_router{};
	input_router.BindMouseLook(camera, ui_captures_mouse, settings);
	const glm::vec3 forward_before_delta = camera.GetForward();

	const std::optional<ve::engine::MouseLookDelta> first_sample =
		input_router.HandleCursorPosition(10.0, 10.0);
	const std::optional<ve::engine::MouseLookDelta> second_sample =
		input_router.HandleCursorPosition(12.0, 8.0);

	CHECK(!first_sample.has_value());
	REQUIRE(second_sample.has_value());
	CHECK(second_sample->yaw_degrees == doctest::Approx(1.0f));
	CHECK(second_sample->pitch_degrees == doctest::Approx(1.0f));
	CHECK(glm::length(camera.GetForward() - forward_before_delta) > 0.001f);
}

TEST_CASE("runtime input router resets mouse look while ui captures mouse")
{
	Camera camera{};
	bool ui_captures_mouse = false;
	const ve::gameplay::MouseLookSettings settings{};
	ve::engine::RuntimeInputRouter input_router{};
	input_router.BindMouseLook(camera, ui_captures_mouse, settings);

	(void)input_router.HandleCursorPosition(1.0, 1.0);
	ui_captures_mouse = true;
	CHECK(!input_router.HandleCursorPosition(100.0, 100.0).has_value());
	ui_captures_mouse = false;
	CHECK(!input_router.HandleCursorPosition(200.0, 200.0).has_value());

	const std::optional<ve::engine::MouseLookDelta> next_delta =
		input_router.HandleCursorPosition(210.0, 190.0);
	REQUIRE(next_delta.has_value());
	CHECK(next_delta->yaw_degrees == doctest::Approx(1.0f));
	CHECK(next_delta->pitch_degrees == doctest::Approx(1.0f));
}

TEST_CASE("runtime input router ignores cursor movement without a camera binding")
{
	ve::engine::RuntimeInputRouter input_router{};

	CHECK(!input_router.HandleCursorPosition(10.0, 10.0).has_value());
}
