
#include "voxel/EngineConfig.h"

#include <memory>
#include <string>
#include <vector>

namespace voxel
{
	/** Structured startup failure reason for EngineStartResult. */
	enum class EngineStartFailure
	{
		None,
		InvalidConfiguration,
		RuntimeUnavailable,
		RuntimeStartupFailed
	};

	/** Structured result returned by Engine::StartDetailed. */
	struct EngineStartResult
	{
		EngineStartFailure failure = EngineStartFailure::None;
		std::vector<std::string> issues;
		std::string message;

		/** Creates a successful startup result. */
		[[nodiscard]] static EngineStartResult Success();

		/** Creates a validation-failure startup result with public issue messages. */
		[[nodiscard]] static EngineStartResult InvalidConfiguration(std::vector<std::string> issues);

		/** Creates a startup result for missing runtime capabilities. */
		[[nodiscard]] static EngineStartResult RuntimeUnavailable(std::string message);

		/** Creates a startup result for runtime initialization failure. */
		[[nodiscard]] static EngineStartResult RuntimeStartupFailed(std::string message);

		/** Returns true only for successful startup. */
		[[nodiscard]] explicit operator bool() const noexcept;
	};

	/** Owns an engine runtime instance for either self-driven or host-driven loops. */
	class Engine
	{
	public:
		/** Creates an engine wrapper with a copied public configuration. */
		explicit Engine(EngineConfig config = {});

		/** Releases any active runtime before destroying the wrapper. */
		~Engine();

		Engine(const Engine&) = delete;
