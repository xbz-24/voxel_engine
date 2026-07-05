TEST_CASE("mouse look input consumes cursor movement with runtime settings")
{
	ve::engine::MouseLookState mouse_look_state{};
	ve::gameplay::MouseLookSettings settings{};

	const std::optional<ve::engine::MouseLookDelta> first_sample =
		ve::engine::ConsumeMouseLookDelta(mouse_look_state, 100.0, 100.0, settings);
	const std::optional<ve::engine::MouseLookDelta> second_sample =
		ve::engine::ConsumeMouseLookDelta(mouse_look_state, 110.0, 90.0, settings);

	CHECK(!first_sample.has_value());
	REQUIRE(second_sample.has_value());
	CHECK(second_sample->yaw_degrees == doctest::Approx(1.0f));
	CHECK(second_sample->pitch_degrees == doctest::Approx(1.0f));
}

TEST_CASE("mouse look input supports inversion and clamps negative sensitivity")
{
	ve::engine::MouseLookState mouse_look_state{};
	ve::gameplay::MouseLookSettings settings{};
	settings.sensitivity_degrees_per_pixel = 0.5f;
	settings.invert_x_axis = true;
	settings.invert_y_axis = true;

	(void)ve::engine::ConsumeMouseLookDelta(mouse_look_state, 10.0, 10.0, settings);
	const std::optional<ve::engine::MouseLookDelta> inverted_delta =
		ve::engine::ConsumeMouseLookDelta(mouse_look_state, 12.0, 8.0, settings);

	REQUIRE(inverted_delta.has_value());
	CHECK(inverted_delta->yaw_degrees == doctest::Approx(-1.0f));
	CHECK(inverted_delta->pitch_degrees == doctest::Approx(-1.0f));

	settings.sensitivity_degrees_per_pixel = -1.0f;
	const std::optional<ve::engine::MouseLookDelta> clamped_delta =
		ve::engine::ConsumeMouseLookDelta(mouse_look_state, 14.0, 6.0, settings);

	REQUIRE(clamped_delta.has_value());
	CHECK(clamped_delta->yaw_degrees == doctest::Approx(0.0f));
	CHECK(clamped_delta->pitch_degrees == doctest::Approx(0.0f));
}

TEST_CASE("mouse look input reset discards the next cursor jump")
{
	ve::engine::MouseLookState mouse_look_state{};
	const ve::gameplay::MouseLookSettings settings{};

	(void)ve::engine::ConsumeMouseLookDelta(mouse_look_state, 1.0, 1.0, settings);
	ve::engine::ResetMouseLookState(mouse_look_state);
	const std::optional<ve::engine::MouseLookDelta> reset_sample =
		ve::engine::ConsumeMouseLookDelta(mouse_look_state, 1000.0, 1000.0, settings);

	CHECK(!reset_sample.has_value());
}
