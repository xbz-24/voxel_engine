target_link_libraries(ve_render PUBLIC ve_core)
target_link_libraries(ve_world PUBLIC ve_core ve_render)
target_link_libraries(ve_network PUBLIC ve_core ve_world)
target_link_libraries(ve_app PUBLIC ve_core ve_render ve_render_backends ve_world ve_network)

target_link_libraries(ve_core PUBLIC
    glm::glm
    spdlog::spdlog
    xsimd
)

target_include_directories(ve_render SYSTEM PRIVATE ${Stb_INCLUDE_DIR})
target_include_directories(ve_render SYSTEM PUBLIC ${GLEW_INCLUDE_DIRS})
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
    set(ve_vcpkg_roots)
    if (DEFINED Z_VCPKG_ROOT_DIR)
        list(APPEND ve_vcpkg_roots "${Z_VCPKG_ROOT_DIR}")
    endif()
    get_filename_component(VE_VCPKG_BUILDSYSTEM_DIR "${CMAKE_TOOLCHAIN_FILE}" DIRECTORY)
    get_filename_component(VE_VCPKG_SCRIPTS_DIR "${VE_VCPKG_BUILDSYSTEM_DIR}" DIRECTORY)
    get_filename_component(VE_VCPKG_ROOT "${VE_VCPKG_SCRIPTS_DIR}" DIRECTORY)
    list(APPEND ve_vcpkg_roots "${VE_VCPKG_ROOT}")
    list(REMOVE_DUPLICATES ve_vcpkg_roots)

    set(ve_imgui_backend_dirs)
    foreach(ve_vcpkg_root IN LISTS ve_vcpkg_roots)
        file(GLOB ve_imgui_root_backend_dirs "${ve_vcpkg_root}/buildtrees/imgui/src/*/backends")
        list(APPEND ve_imgui_backend_dirs ${ve_imgui_root_backend_dirs})
    endforeach()
    find_file(ve_imgui_vulkan_backend_source
        NAMES imgui_impl_vulkan.cpp
        PATHS ${ve_imgui_backend_dirs}
        NO_DEFAULT_PATH
        NO_CACHE
    )
    if (ve_imgui_vulkan_backend_source)
        set(VE_IMGUI_VULKAN_BACKEND_SOURCE "${ve_imgui_vulkan_backend_source}" CACHE FILEPATH "Path to imgui_impl_vulkan.cpp" FORCE)
    endif()
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
        message(FATAL_ERROR "imgui_impl_vulkan.cpp was not found in vcpkg buildtrees; run CMake after vcpkg restores imgui.")
    endif()
endif()

target_link_libraries(ve_world PUBLIC
    glm::glm
)

target_link_libraries(ve_network PUBLIC
    asio::asio
    ws2_32
)

target_link_libraries(ve_app PUBLIC
    GLEW::GLEW
    glfw
    glm::glm
    imgui::imgui
)
