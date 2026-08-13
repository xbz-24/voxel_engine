cmake_minimum_required(VERSION 3.25)

project(VoxelEngineAuthoringConsumer LANGUAGES CXX)

find_package(VoxelEngine 0.5.0 EXACT CONFIG REQUIRED COMPONENTS Authoring)
if (TARGET VoxelEngine::SDK)
    message(FATAL_ERROR "The build-tree-only runtime SDK leaked into the Authoring package")
endif()

add_executable(authoring_consumer main.cpp)
target_link_libraries(authoring_consumer PRIVATE VoxelEngine::Authoring)

enable_testing()
add_test(NAME authoring_consumer COMMAND authoring_consumer)
