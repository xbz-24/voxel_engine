cmake_minimum_required(VERSION 3.25)

project(VoxelEngine LANGUAGES CXX)

# Public SDK packaging is tracked in docs/ArchitectureRoadmap.md while the SDK
# still bridges into private runtime/application targets.
# Keep the CMake feature model on C++23 until every supported generator knows
# C++26. VE_USE_LATEST_CXX opts compilers into /std:c++latest or -std=c++2c.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include("${PROJECT_SOURCE_DIR}/cmake/ProjectOptions.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/Dependencies.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/SubsystemTargets.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/PublicSdk.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/VulkanShaders.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/TargetLinks.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/Applications.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/Examples.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/RuntimeSmokeTests.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/ApiDocs.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/EngineTests.cmake")
