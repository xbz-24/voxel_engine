#include "VulkanGpuChunkRenderer.h"

#include <fstream>
#include <vector>

namespace ve::rendering
{
	namespace
	{
		std::vector<char> ReadBinaryFile(const std::filesystem::path& path)
		{
			std::ifstream file(path, std::ios::ate | std::ios::binary);
			if (!file) return {};
			const std::streamsize size = file.tellg();
			if (size <= 0) return {};
			std::vector<char> bytes(static_cast<std::size_t>(size));
			file.seekg(0);
			file.read(bytes.data(), size);
			return bytes;
		}
	}
	VkShaderModule VulkanGpuChunkRenderer::CreateShaderModule(const std::filesystem::path& path) const
	{
		const std::vector<char> code = ReadBinaryFile(path);
		if (code.empty() || code.size() % sizeof(std::uint32_t) != 0u) return VK_NULL_HANDLE;
		VkShaderModuleCreateInfo create_info{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
		create_info.codeSize = code.size();
		create_info.pCode = reinterpret_cast<const std::uint32_t*>(code.data());
		VkShaderModule module = VK_NULL_HANDLE;
		if (vkCreateShaderModule(device_, &create_info, nullptr, &module) != VK_SUCCESS) return VK_NULL_HANDLE;
		return module;
	}
}
