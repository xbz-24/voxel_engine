#include "Engine.h"

#include <cmath>

void Engine::ApplyPlayerPhysics(const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, double frameDeltaTimeSeconds)
{
	if (_isFlying)
	{
		_verticalVelocity = 0.0f;
		_isGrounded = false;
		return;
	}

	constexpr float gravityBlocksPerSecond = 22.0f;
	constexpr float playerEyeHeight = 1.7f;
	glm::vec3 position = camera.GetPosition();
	_verticalVelocity -= gravityBlocksPerSecond * static_cast<float>(frameDeltaTimeSeconds);
	position.y += _verticalVelocity * static_cast<float>(frameDeltaTimeSeconds);

	const int footBlockX = static_cast<int>(std::floor(position.x));
	const int footBlockY = static_cast<int>(std::floor(position.y - playerEyeHeight));
	const int footBlockZ = static_cast<int>(std::floor(position.z));
	if (_verticalVelocity <= 0.0f && blockRegistry.IsSolid(world.GetBlock(footBlockX, footBlockY, footBlockZ)))
	{
		position.y = static_cast<float>(footBlockY) + 1.0f + playerEyeHeight;
		_verticalVelocity = 0.0f;
		_isGrounded = true;
	}
	else
	{
		_isGrounded = false;
	}

	camera.MoveTo(position);
}
