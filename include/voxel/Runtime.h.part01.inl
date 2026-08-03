
#include "voxel/Camera.h"
#include "voxel/WorldTypes.h"

#include <functional>
#include <string>
#include <vector>

namespace voxel
{
	/** Keyboard keys exposed to public frame callbacks. */
	enum class Key
	{
		Unknown,
		W,
		A,
		S,
		D,
		Space,
		Escape,
		F1,
		F2
	};

	/** Semantic input actions mapped from raw runtime input. */
	enum class InputAction
	{
		MoveForward,
		MoveLeft,
		MoveBack,
		MoveRight,
		Jump,
		Cancel,
		ToggleDebugOverlay,
		ToggleRenderMode,
		PrimaryAction
	};

	/** Input state captured for one public update callback frame. */
	struct InputSnapshot
	{
		bool move_forward = false;
		bool move_left = false;
		bool move_back = false;
		bool move_right = false;
		bool jump = false;
		bool escape = false;
		bool f1 = false;
		bool f2 = false;
		bool primary_action = false;
		double mouse_x = 0.0;
		double mouse_y = 0.0;

		/** Returns true when key is down in this frame snapshot. */
		[[nodiscard]] bool IsDown(Key key) const noexcept;

		/** Returns true when action is active in this frame snapshot. */
		[[nodiscard]] bool IsActive(InputAction action) const noexcept;
	};

	/** Camera state exposed to frame callbacks. */
	struct RuntimeCameraState
	{
		Vec3 position{};
		Vec3 forward{};
	};

	/** Block raycast result for the currently selected voxel, if any. */
	struct BlockHitResult
	{
		bool has_hit = false;
		BlockPosition target_block{};
		BlockPosition placement_block{};
		Block target_block_type = Air;
	};

	/** Mutating commands a callback may queue for the runtime to apply after the frame callback returns. */
	struct RuntimeCommands
	{
		std::vector<WorldEdit> world_edits;
		bool request_close = false;

		/** Queues a single block write. */
		RuntimeCommands& SetBlock(BlockPosition position, Block block);

		/** Queues a single block write by coordinates. */
		RuntimeCommands& SetBlock(int block_x, int block_y, int block_z, Block block);

