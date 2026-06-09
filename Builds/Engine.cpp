#include "Engine.h"

#include <filesystem>

Engine::Engine()
	: _currentWindowHeight(0),
	  _currentWindowWidth(0),
	  _cloudDisplayListID(0),
	  _wasLeftMouseButtonPressed(false),
	  _wasRightMouseButtonPressed(false),
	  _wasDebugTogglePressed(false),
	  _wasFlyTogglePressed(false),
	  _wasRenderDistanceDecreasePressed(false),
	  _wasRenderDistanceIncreasePressed(false),
	  _isGrounded(false),
	  _selectedPlacementBlock(ve::blocks::BlockId::Cobblestone)
{
	_applicationSourceFilePath = std::filesystem::absolute(__FILE__);
}

Engine::~Engine() = default;
