#include "voxel/Engine.h"

#include <algorithm>
#include <string>
#include <utility>

namespace voxel
{
	namespace
	{
		[[nodiscard]] float Clamp01(float value) noexcept
		{
			return std::clamp(value, 0.0f, 1.0f);
		}

		[[nodiscard]] Color ClampColor(Color value) noexcept
		{
			return {
				Clamp01(value.r),
				Clamp01(value.g),
				Clamp01(value.b),
				Clamp01(value.a)
			};
		}
	}

	AssetCatalog& AssetCatalog::Texture(std::string name, std::string path)
	{
		textures.push_back(TextureAsset{ std::move(name), std::move(path) });
		return *this;
	}

	AssetCatalog& AssetCatalog::Model(std::string name, std::string path)
	{
		models.push_back(ModelAsset{ std::move(name), std::move(path) });
		return *this;
	}

	AssetCatalog& AssetCatalog::Sound(std::string name, std::string path)
	{
		sounds.push_back(SoundAsset{ std::move(name), std::move(path) });
		return *this;
	}

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

	Light Light::Sun(Vec3 direction, float intensity) noexcept
	{
		Light light{};
		light.kind = LightKind::Directional;
		light.direction = direction;
		light.intensity = std::max(0.0f, intensity);
		return light;
	}

	Light Light::Point(Vec3 position, Color color, float intensity, float range) noexcept
	{
		Light light{};
		light.kind = LightKind::Point;
		light.position = position;
		light.color = ClampColor(color);
		light.intensity = std::max(0.0f, intensity);
		light.range = std::max(0.0f, range);
		return light;
	}

	Environment& Environment::Ambient(Color value) noexcept
	{
		ambient = ClampColor(value);
		return *this;
	}

	Environment& Environment::Fog(Color value, float density) noexcept
	{
		fog = ClampColor(value);
		fog_density = std::max(0.0f, density);
		return *this;
	}

	Environment& Environment::TimeOfDay(float hour) noexcept
	{
		time_of_day = std::clamp(hour, 0.0f, 24.0f);
		return *this;
	}

	MaterialLibrary& MaterialLibrary::Add(Material material)
	{
		materials.push_back(std::move(material));
		return *this;
	}

	Transform Transform::At(Vec3 position) noexcept
	{
		Transform transform{};
		transform.position = position;
		return transform;
	}

	Entity Entity::Named(std::string name)
	{
		Entity entity{};
		entity.name = std::move(name);
		return entity;
	}

	Entity& Entity::At(Vec3 position) noexcept
	{
		transform.position = position;
		return *this;
	}

	Entity& Entity::Rotate(Vec3 euler_degrees) noexcept
	{
		transform.rotation = euler_degrees;
		return *this;
	}

	Entity& Entity::Scale(Vec3 value) noexcept
	{
		transform.scale = value;
		return *this;
	}

	Entity& Entity::Model(std::string asset_name)
	{
		model = std::move(asset_name);
		return *this;
	}

	Entity& Entity::Material(std::string material_name)
	{
		material = std::move(material_name);
		return *this;
	}

	Entity& Entity::Visible(bool enabled) noexcept
	{
		visible = enabled;
		return *this;
	}

	SceneGraph& SceneGraph::Add(Entity entity)
	{
		entities.push_back(std::move(entity));
		return *this;
	}

	SceneGraph& SceneGraph::Add(Light light)
	{
		lights.push_back(light);
		return *this;
	}

	SceneGraph& SceneGraph::EnvironmentSettings(Environment value) noexcept
	{
		environment = value;
		return *this;
	}
}
