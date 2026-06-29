namespace ve::rendering
{
	namespace
	{
		struct LayerSelection
		{
			std::vector<std::string> names;
			std::vector<const char*> pointers;
		};

		std::vector<VkLayerProperties> EnumerateInstanceLayers()
		{
			std::uint32_t layer_count = 0;
			if (vkEnumerateInstanceLayerProperties(&layer_count, nullptr) != VK_SUCCESS) return {};
			std::vector<VkLayerProperties> layers(layer_count);
			if (layer_count > 0 && vkEnumerateInstanceLayerProperties(&layer_count, layers.data()) != VK_SUCCESS) return {};
			return layers;
		}

		std::vector<VkExtensionProperties> EnumerateInstanceExtensions()
		{
			std::uint32_t extension_count = 0;
			if (vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr) != VK_SUCCESS) return {};
			std::vector<VkExtensionProperties> extensions(extension_count);
			if (extension_count > 0 && vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data()) != VK_SUCCESS) return {};
			return extensions;
		}

		bool ContainsLayer(std::span<const VkLayerProperties> layers, const char* name) noexcept
		{
			return std::ranges::any_of(layers, [name](const VkLayerProperties& layer)
			{
				return std::strcmp(layer.layerName, name) == 0;
			});
		}

		bool ContainsExtension(std::span<const VkExtensionProperties> extensions, const char* name) noexcept
		{
			return std::ranges::any_of(extensions, [name](const VkExtensionProperties& extension)
			{
				return std::strcmp(extension.extensionName, name) == 0;
			});
		}

		bool ContainsName(std::span<const std::string> names, const char* name) noexcept
		{
			return std::ranges::any_of(names, [name](const std::string& layer_name)
			{
				return layer_name == name;
			});
		}

		std::string JoinLayerNames(std::span<const VkLayerProperties> layers)
		{
			std::string result;
			for (const VkLayerProperties& layer : layers)
			{
				if (!result.empty()) result += ", ";
				result += layer.layerName;
			}
			return result.empty() ? std::string{ "<none>" } : result;
		}

		std::string JoinNames(std::span<const std::string> names)
		{
			std::string result;
			for (const std::string& name : names)
			{
				if (!result.empty()) result += ", ";
				result += name;
			}
			return result.empty() ? std::string{ "<none>" } : result;
		}

