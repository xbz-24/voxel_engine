cmake_minimum_required(VERSION 3.25)

project(VoxelEngineSdkConsumer LANGUAGES CXX)

find_package(VoxelEngine 0.5.0 EXACT CONFIG REQUIRED COMPONENTS SDK)
if (NOT TARGET VoxelEngine::Authoring OR NOT TARGET VoxelEngine::SDK)
    message(FATAL_ERROR "The installed SDK package is incomplete")
endif()

add_executable(sdk_consumer main.cpp)
target_link_libraries(sdk_consumer PRIVATE VoxelEngine::SDK)

enable_testing()
add_test(NAME sdk_consumer COMMAND sdk_consumer)
