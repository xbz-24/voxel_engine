#pragma once

#include "voxel/Camera.h"

#include <string>

namespace voxel
{
	/** Semantic version of the public C++ SDK surface. */
	struct SdkVersion
	{
		int major = 0;
		int minor = 2;
		int patch = 0;
	};

	/** Feature flags describing which public SDK promises are active in this build. */
	struct SdkFeatures
	{
		bool vulkan_by_default = true;
		bool world_config_serialization = true;
		bool asset_search_roots = true;
		bool asset_catalog_runtime_loading = false;
		bool material_library_runtime_binding = false;
		bool runtime_update_callback = true;
		bool scene_graph_authoring = true;
		bool scene_graph_runtime_rendering = false;
		bool embeddable_frame_loop = true;
		bool configurable_voxel_render_style = true;
		bool directx12_runtime_backend = false;
	};

	/** Returns the SDK version compiled into this header. */
	[[nodiscard]] constexpr SdkVersion Version() noexcept
	{
		return {};
	}

	/** Returns the feature flags compiled into this header. */
	[[nodiscard]] constexpr SdkFeatures Features() noexcept
	{
		return {};
	}

	/** Requested graphics backend for EngineConfig startup. */
	enum class GraphicsApi
	{
		Vulkan,
		OpenGLCompatibility,
		DirectX12
	};

	/** Built-in showcase scene requested by demo helpers. */
	enum class DemoScene
	{
		HyperrealDesert,
		AquaModel,
		SponzaAtrium
	};

	/** Backend-neutral lighting, sky, tone-mapping, and fog settings for voxel rendering. */
	struct VoxelRenderStyle
	{
		Vec3 sun_direction{ -0.42f, 0.78f, -0.46f };
		Vec3 sun_color{ 1.16f, 1.04f, 0.84f };
		Vec3 sky_horizon_color{ 0.72f, 0.70f, 0.64f };
		Vec3 sky_zenith_color{ 0.52f, 0.68f, 0.88f };
		float sun_intensity = 1.0f;
		float exposure = 1.0f;
		float fog_start_distance = 165.0f;
		float fog_end_distance = 455.0f;

		/** Sets the world-space direction toward the sun. */
		VoxelRenderStyle& WithSunDirection(Vec3 direction) noexcept;

		/** Sets the linear sun color; HDR channel values greater than one are allowed. */
		VoxelRenderStyle& WithSunColor(Vec3 color) noexcept;

		/** Sets non-negative direct-sun intensity. */
		VoxelRenderStyle& WithSunIntensity(float intensity) noexcept;

		/** Sets linear horizon and zenith colors used by sky and aerial perspective. */
		VoxelRenderStyle& WithSkyColors(Vec3 horizon, Vec3 zenith) noexcept;

		/** Sets positive tone-mapping exposure. */
		VoxelRenderStyle& WithExposure(float value) noexcept;

		/** Sets the distance interval over which atmospheric fog accumulates. */
		VoxelRenderStyle& WithFogRange(float start_distance, float end_distance) noexcept;
	};

	/** Minimum severity routed through the engine logger. */
	enum class LogLevel
	{
		Default,
		Trace,
		Debug,
		Info,
		Warning,
		Error,
		Fatal
	};

	/** Logging policy translated into the runtime logger at startup. */
	struct LogSettings
	{
		LogLevel minimum_level = LogLevel::Default;
		bool console_enabled = true;
		bool file_output_enabled = true;
		std::string file_output_path;

		/** Sets the lowest severity that should be emitted. */
		LogSettings& WithMinimumLevel(LogLevel level) noexcept;

		/** Enables or disables console log output. */
		LogSettings& EnableConsole(bool enabled = true) noexcept;

		/** Disables console log output. */
		LogSettings& DisableConsole() noexcept;

		/** Enables file output and writes logs to path. */
		LogSettings& WriteToFile(std::string path);

		/** Disables file log output while preserving the configured path string. */
		LogSettings& DisableFileOutput() noexcept;
	};

	/** Window creation settings used by EngineConfig and the command-line demos. */
	struct WindowConfig
	{
		std::string title = "Voxel Engine";
		int width = 1280;
		int height = 720;
		int monitor_index = 0;
		int refresh_rate_hertz = 0;
		bool fullscreen = false;
		bool vsync = false;
		bool resizable = true;
		bool high_dpi_framebuffer = true;
		bool capture_cursor_on_start = false;

		/** Creates a windowed-mode configuration with a title and client size. */
		[[nodiscard]] static WindowConfig Windowed(std::string title, int width = 1280, int height = 720);

		/** Creates a fullscreen configuration using the selected monitor. */
		[[nodiscard]] static WindowConfig Fullscreen(std::string title = "Voxel Engine");

		/** Enables or disables vertical sync for presentation backends that support it. */
		WindowConfig& EnableVSync(bool enabled = true) noexcept;

		/** Selects the display monitor by zero-based index. */
		WindowConfig& UseMonitor(int index) noexcept;

		/** Requests a fullscreen refresh rate; zero keeps the platform default. */
		WindowConfig& WithRefreshRate(int hertz) noexcept;

		/** Controls whether users can resize the created window. */
		WindowConfig& SetResizable(bool enabled = true) noexcept;

		/** Controls high-DPI framebuffer creation where the platform exposes it. */
		WindowConfig& EnableHighDpiFramebuffer(bool enabled = true) noexcept;

		/** Requests initial mouse capture when the runtime starts. */
		WindowConfig& CaptureCursorOnStart(bool enabled = true) noexcept;
	};

	/** Convenience factory for WindowConfig::Windowed. */
	[[nodiscard]] WindowConfig Window(std::string title, int width = 1280, int height = 720);

	/** Convenience factory for WindowConfig::Fullscreen. */
	[[nodiscard]] WindowConfig Fullscreen(std::string title = "Voxel Engine");
}
