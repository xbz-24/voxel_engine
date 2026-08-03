		SceneGraphCompositionMode composition_mode = SceneGraphCompositionMode::IndependentOverlay;
		std::vector<Entity> entities;
		std::vector<Light> lights;
		Environment environment{};

		/** Sets the composition mode. */
		SceneGraph& ComposeAs(SceneGraphCompositionMode mode) noexcept;

		/** Adds an entity and keeps this graph as the return value for fluent chaining. */
		SceneGraph& Add(Entity entity);

		/** Adds an entity and returns the stable id assigned to it. */
		[[nodiscard]] EntityId AddEntity(Entity entity);

		/** Adds an entity parented to parent_id and returns the assigned id. */
		[[nodiscard]] EntityId AddChild(EntityId parent_id, Entity entity);

		/** Finds an entity by id, or returns nullptr when it is absent. */
		[[nodiscard]] Entity* FindEntity(EntityId entity_id) noexcept;

		/** Finds an entity by id, or returns nullptr when it is absent. */
		[[nodiscard]] const Entity* FindEntity(EntityId entity_id) const noexcept;

		/** Adds a light and keeps this graph as the return value for fluent chaining. */
		SceneGraph& Add(Light light);

		/** Replaces scene-wide environment settings. */
		SceneGraph& EnvironmentSettings(Environment value) noexcept;

		/** Returns validation messages for invalid ids, parents, asset references, and light settings. */
		[[nodiscard]] std::vector<std::string> Validate() const;
	};
}
