#pragma once

#include <string>

namespace ve::assets
{
	enum class ModelImportDiagnosticSeverity
	{
		Info,
		Warning,
		Error
	};

	struct ModelImportDiagnostic
	{
		ModelImportDiagnosticSeverity severity = ModelImportDiagnosticSeverity::Info;
		std::string message;
	};
}
