cmake_minimum_required(VERSION 3.25)

project(VoxelEngine)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_definitions(_WIN32_WINNT=0x0A00 NOMINMAX)

option(VE_BUILD_API_DOCS "Build Doxygen API documentation when Doxygen is available" ON)

find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)
find_package(doctest CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

find_package(Stb REQUIRED)

add_executable(main
    main.cpp
    Builds/AsyncChunkMeshScheduler.cpp
    Builds/AsyncWorldGenerator.cpp
    Builds/AssetPaths.cpp
    Builds/AssimpModelImporter.cpp
    Builds/AssimpModelImporterMaterials.cpp
    Builds/AssimpModelImporterMeshes.cpp
    Builds/AmbientOcclusionPass.cpp
    Builds/BackgroundTaskQueue.cpp
    Builds/BackgroundTaskQueueWorkers.cpp
    Builds/BlockInteraction.cpp
    Builds/BlockPbrMaterials.cpp
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
    Builds/ChunkFaceOffsets.cpp
    Builds/ChunkGenerationState.cpp
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
    Builds/ChunkVisibilityCuller.cpp
    Builds/ComputeTerrainGenerator.cpp
    Builds/CascadedShadowMap.cpp
    Builds/DeferredRenderer.cpp
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
    Builds/Entity.cpp
    Builds/EntityRegistry.cpp
    Builds/EditorDemoState.cpp
    Builds/EditorGui.cpp
    Builds/EditorGuiPanel.cpp
    Builds/EditorRuntimeController.cpp
    Builds/EcsWorld.cpp
    Builds/FrameTimer.cpp
    Builds/FrameGraph.cpp
    Builds/FrameGraphBuilder.cpp
    Builds/FabrikInverseKinematicsPasses.cpp
    Builds/FabrikInverseKinematicsSolver.cpp
    Builds/Frustum.cpp
    Builds/GameController.cpp
    Builds/GameModel.cpp
    Builds/GameView.cpp
    Builds/GameplayInput.cpp
    Builds/GreedyChunkMesher.cpp
    Builds/GreedyChunkMesherCollect.cpp
    Builds/GpuOcclusionQuery.cpp
    Builds/GpuDrivenDrawBuffer.cpp
    Builds/GpuDrivenRenderer.cpp
    Builds/Hotbar.cpp
    Builds/HudDebug.cpp
    Builds/HudRenderer.cpp
    Builds/HudSettingsMenu.cpp
    Builds/HudSettingsMenuParts.cpp
    Builds/HudSurvival.cpp
    Builds/HudText.cpp
    Builds/Input.cpp
    Builds/InputMappings.cpp
    Builds/InverseKinematicsChain.cpp
    Builds/LevelSpawn.cpp
    Builds/LinearMemoryResource.cpp
    Builds/LogFormatter.cpp
    Builds/Logger.cpp
    Builds/LoggerService.cpp
    Builds/MemoryArena.cpp
    Builds/ModelAssetLibrary.cpp
    Builds/ModelImporterRegistry.cpp
    Builds/MultiplayerClient.cpp
    Builds/MultiplayerClientMessages.cpp
    Builds/MultiplayerServer.cpp
    Builds/MultiplayerServerBroadcast.cpp
    Builds/MultiplayerServerConnections.cpp
    Builds/PbrMaterial.cpp
    Builds/PhysicsBvh.cpp
    Builds/PhysicsBvhQuery.cpp
    Builds/PhysicsBounds.cpp
    Builds/PhysicsOctree.cpp
    Builds/PhysicsOctreeInsert.cpp
    Builds/PhysicsOctreeQuery.cpp
    Builds/NetworkBlockReplication.cpp
    Builds/NetworkAsioRuntime.cpp
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
    Builds/RigidBody.cpp
    Builds/RigidBodyForces.cpp
    Builds/RigidBodyWorld.cpp
    Builds/RigidBodyWorldIntegration.cpp
    Builds/RuntimeSettings.cpp
    Builds/SettingsMenuActions.cpp
    Builds/SettingsMenuController.cpp
    Builds/ScreenStack.cpp
    Builds/SkyBox.cpp
    Builds/SpdlogLoggerBackend.cpp
    Builds/TemporalAntiAliasingPass.cpp
    Builds/TemporalAntiAliasingState.cpp
    Builds/TextureLoader.cpp
    Builds/VolumetricRaymarcher.cpp
    Builds/VoxelConeTracing.cpp
    Builds/VoxelRenderPipeline.cpp
    Builds/Window.cpp
    Builds/WindowCursor.cpp
    Builds/WindowRuntime.cpp
    Builds/WorkerPolicy.cpp
    Builds/WorldBlockAccess.cpp
    Builds/WorldChunkLookup.cpp
    Builds/WorldChunkMeshScheduling.cpp
    Builds/WorldChunkMeshing.cpp
    Builds/WorldConfiguration.cpp
    Builds/WorldCoordinates.cpp
    Builds/WorldEvents.cpp
    Builds/WorldGeneration.cpp
    Builds/WorldReplicationController.cpp
    Builds/WorldRender.cpp
    Builds/WorldSpawn.cpp
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
    assimp::assimp
    asio::asio
    imgui::imgui
    spdlog::spdlog
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

option(VE_BUILD_TESTS "Build engine smoke tests" ON)
if (VE_BUILD_TESTS)
    add_executable(engine_tests
        Tests/EngineSmokeTests.cpp
        Builds/Entity.cpp
        Builds/EntityRegistry.cpp
        Builds/EcsWorld.cpp
        Builds/FabrikInverseKinematicsPasses.cpp
        Builds/FabrikInverseKinematicsSolver.cpp
        Builds/InverseKinematicsChain.cpp
        Builds/PhysicsBounds.cpp
        Builds/RigidBody.cpp
        Builds/RigidBodyForces.cpp
        Builds/RigidBodyWorld.cpp
        Builds/RigidBodyWorldIntegration.cpp
    )
    target_include_directories(engine_tests PRIVATE Builds)
    target_link_libraries(engine_tests PRIVATE doctest::doctest glm::glm)
endif()
