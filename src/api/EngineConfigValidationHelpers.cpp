#include "EngineRuntimeBridge.h"

#include <algorithm>
#include <concepts>
#include <cmath>
#include <filesystem>
#include <ranges>
#include <set>
#include <span>
#include <string_view>
#include <type_traits>
#include <utility>

namespace voxel
{
	namespace
	{
		[[nodiscard]] std::string MaterialLabel(const Material& material)
		{
			return material.name.empty() ? "material" : "material '" + material.name + "'";
		}

		[[nodiscard]] std::string EntityLabel(const Entity& entity)
		{
			return entity.name.empty() ? "scene entity" : "scene entity '" + entity.name + "'";
		}

		void ValidateFiniteRange(float value,
			std::string label,
			FloatRange range,
			std::vector<std::string>& issues)
		{
			if (!std::isfinite(value))
			{
				issues.push_back(label + " must be finite");
			}
			else if (!range.Contains(value))
			{
				issues.push_back(label + " must be between 0 and 1");
			}
		}

		void ValidateNonNegative(float value, std::string label, std::vector<std::string>& issues)
		{
			if (!std::isfinite(value))
			{
				issues.push_back(label + " must be finite");
			}
			else if (value < MinimumEmissionStrength)
			{
				issues.push_back(label + " must be zero or greater");
			}
		}

		void ValidateColor(Color color, const std::string& label, std::vector<std::string>& issues)
		{
			ValidateFiniteRange(color.r, label + ".r", NormalizedFloatRange, issues);
			ValidateFiniteRange(color.g, label + ".g", NormalizedFloatRange, issues);
			ValidateFiniteRange(color.b, label + ".b", NormalizedFloatRange, issues);
			ValidateFiniteRange(color.a, label + ".a", NormalizedFloatRange, issues);
		}

		template <std::ranges::input_range NamedRange>
		[[nodiscard]] std::set<std::string, std::less<>> CollectNonEmptyNamesFrom(const NamedRange& named_values)
		{
			std::set<std::string, std::less<>> non_empty_names;
			for (const auto& named_value : named_values)
			{
				if (!named_value.name.empty())
				{
					non_empty_names.insert(named_value.name);
				}
			}
			return non_empty_names;
		}

		void ValidateOptionalReference(std::string_view reference,
			const std::set<std::string, std::less<>>& known_names,
			const std::string& owner_label,
			std::string_view reference_kind,
			std::vector<std::string>& issues)
		{
			if (reference.empty() || known_names.contains(reference))
			{
				return;
			}
			issues.push_back(owner_label + " references missing " + std::string{ reference_kind } + ": " +
				std::string{ reference });
		}

	}
}
