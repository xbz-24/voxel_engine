#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <string>

TEST_CASE("public runtime callbacks expose frame diagnostics and logging")
{
	bool update_called = false;
	bool diagnostics_called = false;
	bool log_called = false;
	int pending_mesh_tasks = 0;
	int pending_mesh_uploads = 0;

	const voxel::EngineConfig config = voxel::EngineConfig::Default()
		.OnUpdate([&update_called](voxel::FrameContext& frame) {
			update_called = true;
			CHECK_FALSE(frame.input.IsDown(voxel::Key::Unknown));
			frame.commands.SetBlock(1, 64, 1, voxel::SeaLantern)
				.ClearBox(2, 64, 2, 3, 65, 3)
				.RequestClose();
		})
		.OnDiagnostics([&](const voxel::Diagnostics& diagnostics) {
			diagnostics_called = true;
			CHECK(diagnostics.render_distance_chunks >= 0);
			pending_mesh_tasks = diagnostics.pending_chunk_mesh_tasks;
			pending_mesh_uploads = diagnostics.pending_chunk_mesh_uploads;
		})
		.OnLog([&log_called](const std::string& line) {
			log_called = true;
			CHECK_FALSE(line.empty());
		});

	voxel::FrameContext frame{};
	frame.input.f1 = true;
	frame.input.primary_action = true;
	frame.camera.position = { 1.0f, 2.0f, 3.0f };
	frame.camera.forward = { 0.0f, 0.0f, -1.0f };
	frame.selected_block = voxel::Cobblestone;
	frame.hit_result = voxel::BlockHitResult{
		true, voxel::At(1, 64, 1), voxel::At(1, 65, 1), voxel::Stone
	};
	config.on_update(frame);

	CHECK(update_called);
	CHECK(frame.input.IsDown(voxel::Key::F1));
	CHECK(frame.input.IsActive(voxel::InputAction::ToggleDebugOverlay));
	CHECK(frame.input.IsActive(voxel::InputAction::PrimaryAction));
	CHECK(frame.camera.position.y == doctest::Approx(2.0f));
	CHECK(frame.camera.forward.z == doctest::Approx(-1.0f));
	CHECK(frame.selected_block == voxel::Cobblestone);
	CHECK(frame.hit_result.has_hit);
	CHECK(frame.hit_result.target_block.y == 64);
	CHECK(frame.hit_result.target_block_type == voxel::Stone);
	CHECK(frame.commands.world_edits.size() == 2);
	CHECK(frame.commands.request_close);

	voxel::RuntimeCommands typed_commands{};
	typed_commands.FillBox(voxel::At(5, 64, 5), voxel::At(4, 63, 4), voxel::Stone)
		.ClearBox(voxel::At(5, 65, 5), voxel::At(4, 65, 4));
	REQUIRE(typed_commands.world_edits.size() == 2);
	CHECK(typed_commands.world_edits[0].box.minimum.x == 4);
	CHECK(typed_commands.world_edits[0].box.minimum.y == 63);
	CHECK(typed_commands.world_edits[0].box.maximum.z == 5);
	CHECK(typed_commands.world_edits[1].block == voxel::Air);

	config.on_diagnostics(voxel::Diagnostics{ 60.0, 2, 4, 5, 6, 7 });
	CHECK(diagnostics_called);
	CHECK(pending_mesh_tasks == 5);
	CHECK(pending_mesh_uploads == 6);

	config.on_log("public api log");
	CHECK(log_called);
}
