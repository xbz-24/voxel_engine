#include "EngineRuntimeBridge.h"

#include "CoreTypes.h"

#include <algorithm>
#include <utility>

[[nodiscard]] FrameContext BuildPublicFrameContextFromRuntimeFrame(
			const ve::engine::RuntimeFrameContext& runtime_frame)
		{
			FrameContext public_frame{};
			public_frame.delta_seconds = runtime_frame.delta_seconds;
			public_frame.elapsed_seconds = runtime_frame.elapsed_seconds;
			public_frame.fps = runtime_frame.fps;
			public_frame.input.move_forward = runtime_frame.input.move_forward;
			public_frame.input.move_left = runtime_frame.input.move_left;
			public_frame.input.move_back = runtime_frame.input.move_back;
			public_frame.input.move_right = runtime_frame.input.move_right;
			public_frame.input.jump = runtime_frame.input.jump;
			public_frame.input.escape = runtime_frame.input.escape;
			public_frame.input.f1 = runtime_frame.input.f1;
			public_frame.input.f2 = runtime_frame.input.f2;
			public_frame.input.primary_action = runtime_frame.input.primary_action;
			public_frame.input.mouse_x = runtime_frame.input.mouse_x;
			public_frame.input.mouse_y = runtime_frame.input.mouse_y;
			public_frame.camera.position = ToPublicVec3(runtime_frame.camera.position);
			public_frame.camera.forward = ToPublicVec3(runtime_frame.camera.forward);
			public_frame.selected_block = ToPublicBlock(runtime_frame.selected_block);
			public_frame.hit_result.has_hit = runtime_frame.hit_result.has_hit;
			public_frame.hit_result.target_block = ToPublicBlockPosition(runtime_frame.hit_result.target_block);
			public_frame.hit_result.placement_block = ToPublicBlockPosition(runtime_frame.hit_result.placement_block);
			public_frame.hit_result.target_block_type = ToPublicBlock(runtime_frame.hit_result.target_block_id);
			return public_frame;
		}

		void AppendPublicFrameCommandsToRuntimeFrame(const FrameContext& public_frame,
			ve::engine::RuntimeFrameContext& runtime_frame)
		{
			runtime_frame.world_edits.reserve(public_frame.commands.world_edits.size());
			for (const WorldEdit& world_edit : public_frame.commands.world_edits)
			{
				runtime_frame.world_edits.push_back(ToInternalWorldEdit(world_edit));
			}
			runtime_frame.request_close = public_frame.commands.request_close;
		}

		[[nodiscard]] Diagnostics BuildPublicDiagnosticsFromRuntimeDiagnostics(
			const ve::engine::RuntimeDiagnostics& runtime_diagnostics)
		{
			return Diagnostics{
				runtime_diagnostics.fps,
				runtime_diagnostics.pending_world_events,
				runtime_diagnostics.render_distance_chunks,
				runtime_diagnostics.pending_chunk_mesh_tasks,
				runtime_diagnostics.pending_chunk_mesh_uploads,
				runtime_diagnostics.pending_world_generation_tasks
			};
		}

		void BindPublicUpdateCallback(ve::engine::EngineCreateInfo& create_info,
			const UpdateCallback& update_callback)
		{
			if (!update_callback)
			{
				return;
			}

			create_info.on_update = [callback = update_callback](ve::engine::RuntimeFrameContext& runtime_frame) {
				FrameContext public_frame = BuildPublicFrameContextFromRuntimeFrame(runtime_frame);
				callback(public_frame);
				AppendPublicFrameCommandsToRuntimeFrame(public_frame, runtime_frame);
			};
		}

		void BindPublicDiagnosticsCallback(ve::engine::EngineCreateInfo& create_info,
			const DiagnosticsCallback& diagnostics_callback)
		{
			if (!diagnostics_callback)
			{
				return;
			}

			create_info.on_diagnostics =
				[callback = diagnostics_callback](const ve::engine::RuntimeDiagnostics& runtime_diagnostics) {
					callback(BuildPublicDiagnosticsFromRuntimeDiagnostics(runtime_diagnostics));
				};
		}
