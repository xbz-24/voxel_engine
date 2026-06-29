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
