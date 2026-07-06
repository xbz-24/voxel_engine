#include "EngineRuntimeBridge.h"

#include "Engine.h"

#include <memory>
#include <utility>

namespace voxel::detail
{
	namespace
	{
		class ApplicationEngineRuntime final : public IEngineRuntime
		{
		public:
			explicit ApplicationEngineRuntime(ve::engine::ValidatedEngineCreateInfo create_info)
				: engine_(std::move(create_info))
			{
			}

			[[nodiscard]] int Run() override
			{
				return engine_.Run();
			}

			[[nodiscard]] bool Start() override
			{
				return engine_.Start();
			}

			[[nodiscard]] bool Step() override
			{
				return engine_.Step();
			}

			void Shutdown() noexcept override
			{
				engine_.Shutdown();
			}

			void RequestStop() noexcept override
			{
				engine_.RequestStop();
			}

			[[nodiscard]] bool IsRunning() const noexcept override
			{
				return engine_.IsRunning();
			}

		private:
			::EngineApplication engine_;
		};

		class ApplicationEngineRuntimeFactory final : public IEngineRuntimeFactory
		{
		public:
			[[nodiscard]] std::unique_ptr<IEngineRuntime> Create(
				ve::engine::ValidatedEngineCreateInfo create_info) const override
			{
				return std::make_unique<ApplicationEngineRuntime>(std::move(create_info));
			}
		};
	}

	const IEngineRuntimeFactory& DefaultEngineRuntimeFactory() noexcept
	{
		static const ApplicationEngineRuntimeFactory factory{};
		return factory;
	}

	EngineServices DefaultEngineServices() noexcept
	{
		return EngineServices{
			DefaultEngineConfigValidator(),
			DefaultEngineConfigTranslator(),
			DefaultEngineRuntimeFactory()
		};
	}
}
