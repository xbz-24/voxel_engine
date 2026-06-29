target_link_libraries(ve_render PUBLIC ve_core)
target_link_libraries(ve_render_backends PUBLIC ve_core ve_render ve_world)
target_link_libraries(ve_world PUBLIC ve_core ve_render)
target_link_libraries(ve_network PUBLIC ve_core ve_world)
target_link_libraries(ve_app PUBLIC ve_core ve_render ve_render_backends ve_world ve_network)

target_link_libraries(ve_core PUBLIC
    glm::glm
    glfw
    spdlog::spdlog
    xsimd
)

target_include_directories(ve_render SYSTEM PUBLIC ${Stb_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS})
target_link_libraries(ve_render PUBLIC
    GLEW::GLEW
    glm::glm
    assimp::assimp
    volk::volk_headers
    Vulkan::Headers
)

target_link_libraries(ve_render_backends PUBLIC
    Boost::container
    GLEW::GLEW
    glfw
    imgui::imgui
    volk::volk
    volk::volk_headers
    Vulkan::Headers
    GPUOpen::VulkanMemoryAllocator
)

if (CMAKE_TOOLCHAIN_FILE)
    if (NOT VE_IMGUI_VULKAN_BACKEND_SOURCE)
        get_filename_component(VE_VCPKG_BUILDSYSTEM_DIR "${CMAKE_TOOLCHAIN_FILE}" DIRECTORY)
        get_filename_component(VE_VCPKG_SCRIPTS_DIR "${VE_VCPKG_BUILDSYSTEM_DIR}" DIRECTORY)
        get_filename_component(VE_VCPKG_ROOT "${VE_VCPKG_SCRIPTS_DIR}" DIRECTORY)
        file(GLOB VE_IMGUI_BACKEND_DIRS CONFIGURE_DEPENDS "${VE_VCPKG_ROOT}/buildtrees/imgui/src/*/backends")
        find_file(VE_IMGUI_VULKAN_BACKEND_SOURCE imgui_impl_vulkan.cpp PATHS ${VE_IMGUI_BACKEND_DIRS} NO_DEFAULT_PATH)
    endif()
    if (VE_IMGUI_VULKAN_BACKEND_SOURCE)
        get_filename_component(VE_IMGUI_VULKAN_BACKEND_DIR "${VE_IMGUI_VULKAN_BACKEND_SOURCE}" DIRECTORY)
        target_sources(ve_render_backends PRIVATE "${VE_IMGUI_VULKAN_BACKEND_SOURCE}")
        target_include_directories(ve_render_backends SYSTEM PRIVATE "${VE_IMGUI_VULKAN_BACKEND_DIR}")
        target_compile_definitions(ve_render_backends PRIVATE IMGUI_IMPL_VULKAN_USE_VOLK)
        set_source_files_properties("${VE_IMGUI_VULKAN_BACKEND_SOURCE}" PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)
        if (MSVC)
            set_source_files_properties("${VE_IMGUI_VULKAN_BACKEND_SOURCE}" PROPERTIES COMPILE_OPTIONS "/W0")
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
