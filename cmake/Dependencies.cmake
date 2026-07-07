find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)
find_package(Boost CONFIG REQUIRED COMPONENTS container)
if (VE_BUILD_TESTS)
    find_package(doctest CONFIG REQUIRED)
endif()
find_package(imgui CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(volk CONFIG REQUIRED)
find_package(VulkanHeaders CONFIG REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(xsimd CONFIG REQUIRED)
find_program(VE_GLSLC_EXECUTABLE glslc
    HINTS
        "$ENV{VULKAN_SDK}/Bin"
        "C:/VulkanSDK/1.4.350.0/Bin"
)
if (NOT VE_GLSLC_EXECUTABLE)
    message(FATAL_ERROR "glslc was not found; install the Vulkan SDK or add glslc to PATH.")
endif()
find_package(Stb QUIET)
if (NOT Stb_FOUND)
    find_path(Stb_INCLUDE_DIR stb_image.h PATHS ${PROJECT_SOURCE_DIR} REQUIRED)
endif()

# Third-party usage requirements live in one place so subsystem targets stay small.
add_library(ve_dependencies INTERFACE)
target_compile_definitions(ve_dependencies INTERFACE
    _WIN32_WINNT=0x0A00
    NOMINMAX
    ROOT_DIR="${PROJECT_SOURCE_DIR}"
)
