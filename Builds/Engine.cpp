#include "Engine.h"

#include <filesystem>

Engine::Engine()
	: _selectedPlacementBlock(ve::blocks::BlockId::Cobblestone)
{
	_applicationSourceFilePath = std::filesystem::absolute(__FILE__);
}

Engine::~Engine() = default;
