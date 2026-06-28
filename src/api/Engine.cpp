#include "voxel/Engine.h"

#include "EngineRuntimeBridge.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace voxel
{
	CameraConfig CameraConfig::At(Vec3 position, Vec3 look_at) noexcept
	{
		return CameraConfig{ position, look_at, true };
	}

	CameraConfig Camera(Vec3 position, Vec3 look_at) noexcept
	{
		return CameraConfig::At(position, look_at);
	}

	WindowConfig WindowConfig::Windowed(std::string title, int width, int height)
	{
		WindowConfig config{};
		config.title = std::move(title);
		config.width = width;
		config.height = height;
		config.fullscreen = false;
		return config;
	}

	WindowConfig WindowConfig::Fullscreen(std::string title)
	{
		WindowConfig config{};
		config.title = std::move(title);
		config.fullscreen = true;
		return config;
	}

	WindowConfig& WindowConfig::EnableVSync(bool enabled) noexcept
	{
		vsync = enabled;
		return *this;
	}

	WindowConfig Window(std::string title, int width, int height)
	{
		return WindowConfig::Windowed(std::move(title), width, height);
	}

	WindowConfig Fullscreen(std::string title)
	{
		return WindowConfig::Fullscreen(std::move(title));
	}

	EngineConfig EngineConfig::Default()
	{
		return {};
	}

	EngineConfig& EngineConfig::WithWindow(WindowConfig value) noexcept
	{
		window = std::move(value);
		return *this;
	}

	EngineConfig& EngineConfig::WithWindow(std::string title, int width, int height)
	{
		return WithWindow(Window(std::move(title), width, height));
	}

	EngineConfig& EngineConfig::WithWorld(WorldConfig value) noexcept
	{
		world = std::move(value);
		return *this;
	}

	EngineConfig& EngineConfig::WithScene(WorldConfig value) noexcept
	{
		return WithWorld(std::move(value));
	}

	EngineConfig& EngineConfig::WithAssets(AssetCatalog value) noexcept
	{
		assets = std::move(value);
		return *this;
	}

	EngineConfig& EngineConfig::WithMaterials(MaterialLibrary value) noexcept
	{
		materials = std::move(value);
		return *this;
	}

	EngineConfig& EngineConfig::WithSceneGraph(SceneGraph value) noexcept
	{
		scene_graph = std::move(value);
		return *this;
	}

	EngineConfig& EngineConfig::WithCamera(CameraConfig value) noexcept
	{
		camera = value;
		return *this;
	}

	EngineConfig& EngineConfig::WithCamera(Vec3 position, Vec3 look_at) noexcept
	{
		return WithCamera(Camera(position, look_at));
	}

	EngineConfig& EngineConfig::UseGraphicsApi(GraphicsApi value) noexcept
	{
		graphics_api = value;
		return *this;
	}

	EngineConfig& EngineConfig::UseVulkan() noexcept
	{
		return UseGraphicsApi(GraphicsApi::Vulkan);
	}

	EngineConfig& EngineConfig::UseOpenGLCompatibility() noexcept
	{
		return UseGraphicsApi(GraphicsApi::OpenGLCompatibility);
	}

	EngineConfig& EngineConfig::UseDirectX12() noexcept
	{
		return UseGraphicsApi(GraphicsApi::DirectX12);
	}

	EngineConfig& EngineConfig::WithDemoScene(DemoScene value) noexcept
	{
		demo_scene = value;
		return *this;
	}

	EngineConfig& EngineConfig::WithWorldSizeChunks(int value) noexcept
	{
		world.size_chunks = value;
		return *this;
	}

	EngineConfig& EngineConfig::WithRenderDistanceChunks(int value) noexcept
	{
		render_distance_chunks = value;
		return *this;
	}

	EngineConfig& EngineConfig::ShowDebugOverlay(bool enabled) noexcept
	{
		show_debug_overlay = enabled;
		return *this;
	}

	EngineConfig& EngineConfig::HideDebugOverlay() noexcept
	{
		return ShowDebugOverlay(false);
	}

	EngineConfig& EngineConfig::OnUpdate(UpdateCallback callback) noexcept
	{
		on_update = std::move(callback);
		return *this;
	}

	EngineConfig& EngineConfig::OnDiagnostics(DiagnosticsCallback callback) noexcept
	{
		on_diagnostics = std::move(callback);
		return *this;
	}

	EngineConfig& EngineConfig::OnLog(LogCallback callback) noexcept
	{
		on_log = std::move(callback);
		return *this;
	}

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

	int Run(EngineConfig config)
	{
		Engine engine{ std::move(config) };
		return engine.Run();
	}

	int Run(WorldConfig world)
	{
		return Run(DesertDemo().WithWorld(std::move(world)));
	}

	int RunScene(WorldConfig scene)
	{
		return Run(std::move(scene));
	}

	int RunDesertDemo()
	{
		return Run(DesertDemo());
	}

	int RunAquaModelDemo()
	{
		return Run(AquaModelDemo());
	}

	int RunSponzaAtriumDemo()
	{
		return Run(SponzaAtriumDemo());
	}

	int RunMinecraftDemo()
	{
		return RunDesertDemo();
	}

	int RunWorldEditDemo()
	{
		return Run(WorldEditDemo());
	}

	int RunShowcaseDemo()
	{
		return Run(ShowcaseDemo());
	}

	int RunArcadeSnakeDemo()
	{
		return Run(ArcadeSnakeDemo());
	}

	int RunArcadePaddleDemo()
	{
		return Run(ArcadePaddleDemo());
	}

	int RunArcadeBlocksDemo()
	{
		return Run(ArcadeBlocksDemo());
	}

	int RunArcadeInvadersDemo()
	{
		return Run(ArcadeInvadersDemo());
	}

	int RunArcadeMazeDemo()
	{
		return Run(ArcadeMazeDemo());
	}
}
