cmake_minimum_required(VERSION 3.25)

project(VoxelEngine)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

find_package(Stb REQUIRED)

add_executable(main
    main.cpp
    Builds/AssetPaths.cpp
    Builds/Camera.cpp
    Builds/Chunk.cpp
    Builds/Cube.cpp
    Builds/Engine.cpp
    Builds/FrameTimer.cpp
    Builds/LevelSpawn.cpp
    Builds/Logger.cpp
    Builds/Plane.cpp
    Builds/Render2D.cpp
    Builds/SkyBox.cpp
    Builds/Utilities.cpp
    Builds/Window.cpp
    Builds/World.cpp
    Builds/dependencies.cpp
)
target_compile_definitions(main PRIVATE ROOT_DIR="${CMAKE_SOURCE_DIR}")

target_link_libraries(main PRIVATE 
    GLEW::GLEW 
    glfw 
    glm::glm
)

target_include_directories(main PRIVATE
    Builds
    ${Stb_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIRS}
)
