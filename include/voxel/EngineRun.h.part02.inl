		Engine& operator=(const Engine&) = delete;
		Engine(Engine&&) noexcept;
		Engine& operator=(Engine&&) noexcept;

		/** @return Process-style status after running the engine-owned main loop. */
		[[nodiscard]] int Run();

		/** @return True when runtime systems were initialized for an externally driven frame loop. */
		[[nodiscard]] bool Start();

		/** @return Structured startup status for hosts that need actionable failure details. */
		[[nodiscard]] EngineStartResult StartDetailed();

		/** @return True when one frame ran and another frame should be requested by the host. */
		[[nodiscard]] bool Step();

		/** Releases runtime systems created by Start(). */
		void Shutdown() noexcept;

		/** Requests the running loop to stop at the next frame boundary. */
		void RequestStop() noexcept;

		/** @return True when Start() succeeded and Shutdown() has not been called yet. */
		[[nodiscard]] bool IsRunning() const noexcept;

	private:
		class Impl;
		std::unique_ptr<Impl> impl_;
	};

	/** Runs the engine-owned main loop and returns a process-style exit code. */
	[[nodiscard]] int Run(EngineConfig config = {});

	/** Runs a world config using default engine settings. */
	[[nodiscard]] int Run(WorldConfig world);

	/** Runs a scene/world config using default engine settings. */
	[[nodiscard]] int RunScene(WorldConfig scene);
}
