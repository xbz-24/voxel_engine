cmake_minimum_required(VERSION 3.25)

project(VoxelEngine)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(VE_BUILD_API_DOCS "Build Doxygen API documentation when Doxygen is available" ON)

find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

find_package(Stb REQUIRED)

add_executable(main
    main.cpp
    Builds/AsyncChunkMeshScheduler.cpp
    Builds/AssetPaths.cpp
    Builds/BackgroundTaskQueue.cpp
    Builds/BackgroundTaskQueueWorkers.cpp
    Builds/BlockInteraction.cpp
    Builds/BlockRaycaster.cpp
    Builds/BlockRegistry.cpp
    Builds/BlockSelectionCube.cpp
    Builds/BlockSelectionCubeFaces.cpp
    Builds/BlockSelectionCubeSideFaces.cpp
    Builds/BlockTextureCache.cpp
    Builds/Camera.cpp
    Builds/Chunk.cpp
    Builds/ChunkDecorations.cpp
    Builds/ChunkFaceEmitter.cpp
    Builds/ChunkGpuMesh.cpp
    Builds/ChunkMeshInput.cpp
    Builds/ChunkMeshBuilder.cpp
    Builds/ChunkMeshCollector.cpp
    Builds/ChunkMeshPipeline.cpp
    Builds/ChunkMeshRequest.cpp
    Builds/ChunkMeshSnapshot.cpp
    Builds/ChunkMeshUploader.cpp
    Builds/ChunkMeshUploadQueue.cpp
    Builds/ChunkMeshing.cpp
    Builds/ChunkTerrain.cpp
    Builds/Engine.cpp
    Builds/EngineClouds.cpp
    Builds/EngineDebugRender.cpp
    Builds/EngineFrame.cpp
    Builds/EnginePlayerBlockInput.cpp
    Builds/EngineLifecycle.cpp
    Builds/EngineCameraMouseInput.cpp
    Builds/EnginePlayerMovement.cpp
    Builds/EnginePhysics.cpp
    Builds/EngineRender.cpp
    Builds/EngineRun.cpp
    Builds/FrameTimer.cpp
    Builds/GameController.cpp
    Builds/GameModel.cpp
    Builds/GameView.cpp
    Builds/GameplayInput.cpp
    Builds/Hotbar.cpp
    Builds/HudDebug.cpp
    Builds/HudRenderer.cpp
    Builds/HudSettingsMenu.cpp
    Builds/HudSettingsMenuParts.cpp
    Builds/HudSurvival.cpp
    Builds/HudText.cpp
    Builds/Input.cpp
    Builds/InputMappings.cpp
    Builds/LevelSpawn.cpp
    Builds/LogFormatter.cpp
    Builds/Logger.cpp
    Builds/LoggerService.cpp
    Builds/MultiplayerClient.cpp
    Builds/MultiplayerClientMessages.cpp
    Builds/MultiplayerServer.cpp
    Builds/MultiplayerServerBroadcast.cpp
    Builds/MultiplayerServerConnections.cpp
    Builds/NetworkBlockReplication.cpp
    Builds/NetworkPacketFraming.cpp
    Builds/NetworkPacketIO.cpp
    Builds/NetworkSerialization.cpp
    Builds/NetworkSession.cpp
    Builds/NetworkSessionPump.cpp
    Builds/NetworkSocketLibrary.cpp
    Builds/NetworkTcpSocket.cpp
    Builds/NetworkTcpSocketConnect.cpp
    Builds/NetworkTcpSocketListen.cpp
    Builds/NetworkTcpSocketTransfer.cpp
    Builds/Plane.cpp
    Builds/PlayerMovementInput.cpp
    Builds/Render2D.cpp
    Builds/RenderPrimitives3D.cpp
    Builds/RenderState.cpp
    Builds/RuntimeSettings.cpp
    Builds/SettingsMenuActions.cpp
    Builds/SettingsMenuController.cpp
    Builds/ScreenStack.cpp
    Builds/SkyBox.cpp
    Builds/TextureLoader.cpp
    Builds/Window.cpp
    Builds/WindowCursor.cpp
    Builds/WindowRuntime.cpp
    Builds/WorldBlockAccess.cpp
    Builds/WorldChunkLookup.cpp
    Builds/WorldChunkMeshing.cpp
    Builds/WorldConfiguration.cpp
    Builds/WorldCoordinates.cpp
    Builds/WorldEvents.cpp
    Builds/WorldReplicationController.cpp
    Builds/WorldRender.cpp
    Builds/WorldVisibility.cpp
    Builds/WorldViewRange.cpp
    Builds/World.cpp
    Builds/dependencies.cpp
)
target_compile_definitions(main PRIVATE ROOT_DIR="${CMAKE_SOURCE_DIR}")

target_link_libraries(main PRIVATE 
    GLEW::GLEW 
    glfw 
    glm::glm
    ws2_32
)

target_include_directories(main PRIVATE
    Builds
    ${Stb_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIRS}
)

if (VE_BUILD_API_DOCS)
    find_package(Doxygen QUIET)
    if (DOXYGEN_FOUND)
        configure_file(${CMAKE_SOURCE_DIR}/Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile @ONLY)
        add_custom_target(engine_api_docs ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Generating VoxelEngine API documentation with Doxygen"
            VERBATIM
        )
    else()
        message(STATUS "Doxygen was not found; API documentation target will be skipped.")
    endif()
endif()
