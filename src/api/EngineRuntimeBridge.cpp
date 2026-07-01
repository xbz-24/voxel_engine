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
			explicit ApplicationEngineRuntime(ve::engine::EngineCreateInfo create_info)
				: engine_(std::move(create_info))
			{
			}

			[[nodiscard]] int Run() override
			{
				return engine_.Run();
			}

			void RequestStop() noexcept override
			{
				engine_.RequestStop();
			}

		private:
			::EngineApplication engine_;
		};

		class ApplicationEngineRuntimeFactory final : public IEngineRuntimeFactory
		{
		public:
			[[nodiscard]] std::unique_ptr<IEngineRuntime> Create(ve::engine::EngineCreateInfo create_info) const override
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
