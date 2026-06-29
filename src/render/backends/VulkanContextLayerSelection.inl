		LayerSelection SelectLayers(const VulkanContextSettings& settings, std::span<const VkLayerProperties> available_layers)
		{
			LayerSelection selection;
			if (settings.enable_all_available_layers)
			{
				selection.names.reserve(available_layers.size() + settings.requested_layers.size());
				for (const VkLayerProperties& layer : available_layers)
				{
					selection.names.emplace_back(layer.layerName);
				}
			}

			if (settings.enable_validation_layers)
			{
				for (const char* requested_layer : settings.requested_layers)
				{
					if (ContainsLayer(available_layers, requested_layer))
					{
						if (!ContainsName(selection.names, requested_layer)) selection.names.emplace_back(requested_layer);
					}
					else
					{
						VE_LOG_CATEGORY_WARNING(ve::log::category::Render, std::string("Requested Vulkan layer is unavailable: ") + requested_layer);
					}
				}
			}

			selection.pointers.reserve(selection.names.size());
			for (const std::string& name : selection.names)
			{
				selection.pointers.push_back(name.c_str());
			}
			return selection;
		}

		void AppendUniqueExtension(std::vector<const char*>& extensions, const char* extension)
		{
			const auto found = std::ranges::find_if(extensions, [extension](const char* enabled)
			{
				return std::strcmp(enabled, extension) == 0;
			});
			if (found == extensions.end()) extensions.push_back(extension);
		}
