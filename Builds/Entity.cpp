#include "Entity.h"

namespace ve::ecs
{
	/// Reports whether the handle points to a non-null entity id.
	bool Entity::IsValid() const noexcept
	{
		return generation != 0;
	}

	/// Compares two entity handles.
	bool operator==(Entity left, Entity right) noexcept
	{
		return left.id == right.id && left.generation == right.generation;
	}
}
