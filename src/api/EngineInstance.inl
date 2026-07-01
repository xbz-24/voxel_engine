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
				runtime_ = services.runtime_factory.Create(services.translator.Translate(config));
			}
		}

	public:
		[[nodiscard]] int Run()
		{
			if (!validation_issues_.empty())
			{
				if (on_log_)
				{
					for (const std::string& issue : validation_issues_)
					{
						on_log_("Invalid EngineConfig: " + issue);
					}
				}
				return -1;
			}
			return runtime_ ? runtime_->Run() : -1;
		}

		void RequestStop() noexcept
		{
			if (runtime_ != nullptr)
			{
				runtime_->RequestStop();
			}
		}

	private:
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

	void Engine::RequestStop() noexcept
	{
		impl_->RequestStop();
	}
