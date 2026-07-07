	Material Material::Named(std::string name)
	{
		Material material{};
		material.name = std::move(name);
		return material;
	}

	Material& Material::BaseColor(Color value) noexcept
	{
		base_color = ClampColor(value);
		return *this;
	}

	Material& Material::Texture(std::string texture_name)
	{
		texture = std::move(texture_name);
		return *this;
	}

	Material& Material::NormalTexture(std::string texture_name)
	{
		normal_texture = std::move(texture_name);
		return *this;
	}

	Material& Material::RoughnessTexture(std::string texture_name)
	{
		roughness_texture = std::move(texture_name);
		return *this;
	}

	Material& Material::MetallicTexture(std::string texture_name)
	{
		metallic_texture = std::move(texture_name);
		return *this;
	}

	Material& Material::OcclusionTexture(std::string texture_name)
	{
		occlusion_texture = std::move(texture_name);
		return *this;
	}

	Material& Material::EmissiveTexture(std::string texture_name)
	{
		emissive_texture = std::move(texture_name);
		return *this;
	}

	Material& Material::Metallic(float value) noexcept
	{
		metallic = Clamp01(value);
		return *this;
	}

	Material& Material::Roughness(float value) noexcept
	{
		roughness = Clamp01(value);
		return *this;
	}

	Material& Material::Emissive(float strength) noexcept
	{
		emission = std::max(0.0f, strength);
		return *this;
	}

	Material& Material::Transparent(bool enabled) noexcept
	{
		transparent = enabled;
		return *this;
	}
