	class Engine::Impl
	{
	public:
		explicit Impl(const EngineConfig& config)
			: Impl(config, detail::DefaultEngineServices())
		{
		}

	private:
		explicit Impl(const EngineConfig& config, detail::EngineServices services)
			: validation_issues_(services.validator.Validate(config)),
			  on_log_(config.on_log)
		{
			if (validation_issues_.empty())
			{
				std::vector<std::string> runtime_create_info_issues;
				std::optional<ve::engine::ValidatedEngineCreateInfo> runtime_create_info =
					ve::engine::MakeValidatedEngineCreateInfo(
						services.translator.Translate(config),
						&runtime_create_info_issues);
				validation_issues_.insert(
					validation_issues_.end(),
					runtime_create_info_issues.begin(),
					runtime_create_info_issues.end());
				if (runtime_create_info.has_value())
				{
					runtime_ = services.runtime_factory.Create(std::move(*runtime_create_info));
				}
			}
		}

	public:
		[[nodiscard]] int Run()
		{
			if (!validation_issues_.empty())
			{
				LogValidationIssues();
				return -1;
			}
			return runtime_ ? runtime_->Run() : -1;
		}

		[[nodiscard]] bool Start()
		{
			return static_cast<bool>(StartDetailed());
		}

		[[nodiscard]] EngineStartResult StartDetailed()
		{
			if (!validation_issues_.empty())
			{
				LogValidationIssues();
				return EngineStartResult::InvalidConfiguration(validation_issues_);
			}
			if (runtime_ == nullptr)
			{
				return EngineStartResult::RuntimeUnavailable("Engine runtime was not created");
			}
			const ve::engine::EngineStartupResult startup_result = runtime_->Start();
			if (startup_result)
			{
				return EngineStartResult::Success();
			}
			return EngineStartResult::RuntimeStartupFailed(startup_result.message);
		}

		[[nodiscard]] bool Step()
		{
			if (!validation_issues_.empty() || runtime_ == nullptr)
			{
				return false;
			}
			return runtime_->Step();
		}

		void Shutdown() noexcept
		{
			if (runtime_ != nullptr)
			{
				runtime_->Shutdown();
			}
		}

		void RequestStop() noexcept
		{
			if (runtime_ != nullptr)
			{
				runtime_->RequestStop();
			}
		}

		[[nodiscard]] bool IsRunning() const noexcept
		{
			return runtime_ != nullptr && runtime_->IsRunning();
		}

	private:
		void LogValidationIssues() const
		{
			if (!on_log_) return;
			for (const std::string& issue : validation_issues_)
			{
				on_log_("Invalid EngineConfig: " + issue);
			}
		}

		std::vector<std::string> validation_issues_;
		LogCallback on_log_;
		std::unique_ptr<detail::IEngineRuntime> runtime_;
	};

	Engine::Engine(EngineConfig config)
		: impl_(std::make_unique<Impl>(config))
	{
	}

	Engine::~Engine() = default;

	Engine::Engine(Engine&&) noexcept = default;

	Engine& Engine::operator=(Engine&&) noexcept = default;

	int Engine::Run()
	{
		return impl_->Run();
	}

	bool Engine::Start()
	{
		return impl_->Start();
	}

	EngineStartResult Engine::StartDetailed()
	{
		return impl_->StartDetailed();
	}

	bool Engine::Step()
	{
		return impl_->Step();
	}

	void Engine::Shutdown() noexcept
	{
		impl_->Shutdown();
	}

	void Engine::RequestStop() noexcept
	{
		impl_->RequestStop();
	}

	bool Engine::IsRunning() const noexcept
	{
		return impl_->IsRunning();
	}
