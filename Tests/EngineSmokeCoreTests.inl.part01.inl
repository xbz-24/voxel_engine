TEST_CASE("ecs registry creates and destroys alive entities")
{
	ve::ecs::EcsWorld world;
	const ve::ecs::Entity entity = world.CreateEntity();

	CHECK(entity.IsValid());
	CHECK(world.IsAlive(entity));
	CHECK(world.AliveCount() == 1);
	CHECK(world.DestroyEntity(entity));
	CHECK(world.AliveCount() == 0);
}

TEST_CASE("ecs world stores components behind generation-safe entity handles")
{
	struct PositionComponent
	{
		glm::vec3 value{ 0.0f };
	};

	ve::ecs::EcsWorld world;
	const ve::ecs::Entity entity = world.CreateEntity();
	PositionComponent* position = world.EmplaceComponent<PositionComponent>(
		entity,
		PositionComponent{ glm::vec3{ 1.0f, 2.0f, 3.0f } });

	REQUIRE(position != nullptr);
	CHECK(world.HasComponent<PositionComponent>(entity));
	CHECK(world.ComponentCount<PositionComponent>() == 1U);
	CHECK(world.FindComponent<PositionComponent>(entity)->value.y == doctest::Approx(2.0f));

	CHECK(world.DestroyEntity(entity));
	CHECK(!world.HasComponent<PositionComponent>(entity));
	CHECK(world.ComponentCount<PositionComponent>() == 0U);

	const ve::ecs::Entity recycled_entity = world.CreateEntity();
	CHECK(recycled_entity.id == entity.id);
	CHECK(recycled_entity.generation != entity.generation);
	CHECK(world.FindComponent<PositionComponent>(entity) == nullptr);
}

TEST_CASE("physics bounds report overlapping boxes")
{
	const ve::physics::Aabb left{ glm::vec3{ 0.0f }, glm::vec3{ 2.0f } };
	const ve::physics::Aabb right{ glm::vec3{ 1.0f }, glm::vec3{ 3.0f } };

	CHECK(left.Intersects(right));
	CHECK(left.Contains({ 1.0f, 1.0f, 1.0f }));
}

TEST_CASE("physics bvh exposes parent depth metadata")
{
	const std::array<ve::physics::PhysicsProxy, 4> proxies{
		ve::physics::PhysicsProxy{ 11, { { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } } },
		ve::physics::PhysicsProxy{ 22, { { 3.0f, 0.0f, 0.0f }, { 4.0f, 1.0f, 1.0f } } },
		ve::physics::PhysicsProxy{ 33, { { 0.0f, 3.0f, 0.0f }, { 1.0f, 4.0f, 1.0f } } },
		ve::physics::PhysicsProxy{ 44, { { 3.0f, 3.0f, 0.0f }, { 4.0f, 4.0f, 1.0f } } }
	};
	ve::physics::PhysicsBvh bvh;

	bvh.Build(proxies);

	CHECK(!bvh.IsEmpty());
	CHECK(bvh.NodeCount() == 7);
	const ve::physics::PhysicsBvh::NodeDebugInfo root = bvh.DebugNode(0);
	CHECK(root.parent == -1);
	CHECK(root.depth == 0);
	CHECK(!root.is_leaf);

	for (ve::core::Index node_index = 1; node_index < bvh.NodeCount(); ++node_index)
	{
		const ve::physics::PhysicsBvh::NodeDebugInfo node = bvh.DebugNode(node_index);
		REQUIRE(node.parent >= 0);
		const ve::physics::PhysicsBvh::NodeDebugInfo parent = bvh.DebugNode(ve::core::ToIndex(node.parent));
		CHECK(node.depth == parent.depth + 1);
	}

	ve::core::DynamicArray<unsigned int> overlapped_ids;
	bvh.QueryOverlaps({ { -0.5f, -0.5f, -0.5f }, { 1.5f, 1.5f, 1.5f } }, overlapped_ids);
	REQUIRE(overlapped_ids.size() == 1);
	CHECK(overlapped_ids.front() == 11);

	CHECK(bvh.UpdateProxyBounds(22, { { 0.25f, 0.25f, 0.25f }, { 0.75f, 0.75f, 0.75f } }));
	overlapped_ids.clear();
	bvh.QueryOverlaps({ { -0.5f, -0.5f, -0.5f }, { 1.5f, 1.5f, 1.5f } }, overlapped_ids);
	CHECK(overlapped_ids.size() == 2U);
}

TEST_CASE("rigid body world integrates gravity")
