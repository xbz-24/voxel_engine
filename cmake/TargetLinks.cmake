target_link_libraries(ve_render PUBLIC ve_core)
target_link_libraries(ve_world PUBLIC ve_core ve_render)
target_link_libraries(ve_network PUBLIC ve_core ve_world)
target_link_libraries(ve_runtime PUBLIC ve_core ve_render ve_render_backend_window)
target_link_libraries(ve_voxel_sandbox PUBLIC ve_runtime ve_core ve_render ve_render_backends ve_world)

target_link_libraries(ve_core PUBLIC
    glm::glm
    spdlog::spdlog
    xsimd
)

target_include_directories(ve_render SYSTEM PRIVATE ${Stb_INCLUDE_DIR})
target_link_libraries(ve_render PUBLIC
    GLEW::GLEW
    glm::glm
    volk::volk_headers
    Vulkan::Headers
)
target_link_libraries(ve_render PRIVATE
    assimp::assimp
)

set(ve_imgui_vulkan_backend_source "${VE_IMGUI_VULKAN_BACKEND_SOURCE}")
if (CMAKE_TOOLCHAIN_FILE AND NOT ve_imgui_vulkan_backend_source)
    # Binary-cache restores do not include vcpkg's temporary buildtrees.
    # Match the ImGui source and checksum pinned by our vcpkg baseline.
    get_target_property(ve_imgui_include_dirs imgui::imgui INTERFACE_INCLUDE_DIRECTORIES)
    find_file(ve_imgui_header NAMES imgui.h PATHS ${ve_imgui_include_dirs}
        NO_DEFAULT_PATH NO_CACHE REQUIRED)
    file(STRINGS "${ve_imgui_header}" ve_imgui_version
        REGEX "^#define[ \t]+IMGUI_VERSION[ \t]+")
    if (NOT ve_imgui_version MATCHES "\"1\\.91\\.9\"")
        message(FATAL_ERROR
            "The Vulkan backend requires ImGui 1.91.9; update its source pin with the vcpkg baseline.")
    endif()

    include(FetchContent)
    FetchContent_Declare(ve_imgui_backend
        URL "https://github.com/ocornut/imgui/archive/refs/tags/v1.91.9.tar.gz"
        URL_HASH SHA512=c9393bd9f6b49b036ad6ab3ba4d972876c6f60ce7f5c13a7a56ff11b3559ea3211b0caa03eed10b4f4fbe9c371e14f7f24866bd476652f543f3ed3aa878ea930
        DOWNLOAD_EXTRACT_TIMESTAMP FALSE
        SOURCE_SUBDIR backends
    )
    FetchContent_MakeAvailable(ve_imgui_backend)
    set(ve_imgui_vulkan_backend_source "${ve_imgui_backend_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp")
endif()
if (CMAKE_TOOLCHAIN_FILE)
    if (ve_imgui_vulkan_backend_source)
        get_filename_component(VE_IMGUI_VULKAN_BACKEND_DIR "${ve_imgui_vulkan_backend_source}" DIRECTORY)
        target_sources(ve_render_backend_vulkan PRIVATE "${ve_imgui_vulkan_backend_source}")
        target_include_directories(ve_render_backend_vulkan SYSTEM PRIVATE "${VE_IMGUI_VULKAN_BACKEND_DIR}")
        target_compile_definitions(ve_render_backend_vulkan PRIVATE IMGUI_IMPL_VULKAN_USE_VOLK)
        set_source_files_properties("${ve_imgui_vulkan_backend_source}" PROPERTIES
            SKIP_UNITY_BUILD_INCLUSION ON
            SKIP_PRECOMPILE_HEADERS ON
        )
        if (MSVC)
            set_source_files_properties("${ve_imgui_vulkan_backend_source}" PROPERTIES COMPILE_OPTIONS "/W0;/WX-")
        endif()
    else()
        message(FATAL_ERROR "imgui_impl_vulkan.cpp is required to build the Vulkan backend.")
    endif()
endif()

target_link_libraries(ve_world PUBLIC
    glm::glm
)

target_link_libraries(ve_network PUBLIC
    asio::asio
)

target_link_libraries(ve_voxel_sandbox PUBLIC
    GLEW::GLEW
    glfw
    glm::glm
    imgui::imgui
)
