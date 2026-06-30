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
		const ve::physics::PhysicsBvh::NodeDebugInfo parent = bvh.DebugNode(static_cast<ve::core::Index>(node.parent));
		CHECK(node.depth == parent.depth + 1);
	}

	ve::core::DynamicArray<unsigned int> overlapped_ids;
	bvh.QueryOverlaps({ { -0.5f, -0.5f, -0.5f }, { 1.5f, 1.5f, 1.5f } }, overlapped_ids);
	REQUIRE(overlapped_ids.size() == 1);
	CHECK(overlapped_ids.front() == 11);
}

TEST_CASE("rigid body world integrates gravity")
{
	ve::physics::RigidBodyWorld world({ 0.0f, -10.0f, 0.0f });
	const auto handle = world.CreateBody({ .position = { 0.0f, 10.0f, 0.0f } });

	world.Step(0.1f, 2);
	const ve::physics::RigidBody* body = world.FindBody(handle);

	REQUIRE(body != nullptr);
	CHECK(body->Position().y < 10.0f);
}

TEST_CASE("fabrik inverse kinematics moves end effector toward target")
{
	std::array<glm::vec3, 3> joints{ glm::vec3{ 0.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f }, glm::vec3{ 2.0f, 0.0f, 0.0f } };
	ve::animation::InverseKinematicsChain chain;
	chain.SetJointPositions(joints);

	const ve::animation::FabrikInverseKinematicsSolver solver;
	const bool solved = solver.Solve(chain, { 1.0f, 1.0f, 0.0f });

	CHECK(solved);
	CHECK(glm::distance(chain.EndEffectorPosition(), glm::vec3{ 1.0f, 1.0f, 0.0f }) < 0.05f);
}

TEST_CASE("navier stokes fluid simulation preserves injected density")
{
	ve::simulation::NavierStokesFluidSimulation fluid({ .width = 8, .height = 8 });

	fluid.AddDensity(4, 4, 10.0f);
	fluid.AddVelocity(4, 4, { 1.0f, 0.0f });
	fluid.Step(0.016f);

	CHECK(fluid.Grid().DensityAt(4, 4) > 0.0f);
}

TEST_CASE("monte carlo path tracer sees emissive geometry")
{
	ve::rendering::PathTracingScene scene;
	scene.AddSphere({ .center = { 0.0f, 0.0f, -3.0f }, .radius = 1.0f, .material = { .emission = { 4.0f, 4.0f, 4.0f } } });
	const ve::rendering::MonteCarloPathTracer tracer;

	const ve::rendering::PathRay camera_ray{ glm::vec3{ 0.0f }, glm::vec3{ 0.0f, 0.0f, -1.0f } };
	const glm::vec3 radiance = tracer.TraceRay(scene, camera_ray, 7);

	CHECK(radiance.x > 1.0f);
}

TEST_CASE("simd float4 dot product uses packed math")
{
	const ve::math::SimdFloat4 left{ { 1.0f, 2.0f, 3.0f, 4.0f } };
	const ve::math::SimdFloat4 right{ { 5.0f, 6.0f, 7.0f, 8.0f } };

	CHECK(ve::math::SimdFloat4Math::Dot(left, right) == doctest::Approx(70.0f));
}

TEST_CASE("background task queue runs enqueued work")
{
	ve::tasks::BackgroundTaskQueue queue(1);
	std::atomic_bool completed = false;

	REQUIRE(queue.Enqueue([&completed] { completed.store(true); }));

	for (int attempt = 0; attempt < 100 && !completed.load(); ++attempt)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	CHECK(completed.load());
}

TEST_CASE("background task queue rejects work after stop")
{
	ve::tasks::BackgroundTaskQueue queue(1);
	queue.Stop();

	CHECK(!queue.Enqueue([] {}));
}
