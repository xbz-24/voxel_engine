#include "EditorDemoState.h"

namespace ve::editor
{
	/**
	 * @brief Retrieves the human-readable UI label for a given demo state.
	 *
	 * @note This implementation intentionally omits a `default` case in the switch
	 * statement to enforce compile-time exhaustiveness warnings (e.g., -Wswitch).
	 *
	 * @param demo The demo game identifier to evaluate.
	 * @return A null-terminated string literal representing the demo's name.
	 * Returns "Unknown" if the enum value is invalid or out of bounds.
	 */

	const char* DemoGameName(DemoGame demo) noexcept
	{
		switch (demo)
		{
		case DemoGame::HyperrealDesert: return "Hyperreal voxel desert";
		}
		return "Unknown";
	}
}
