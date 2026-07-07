set(VE_SOURCE_ROOT "${PROJECT_SOURCE_DIR}/src")
set(VE_PUBLIC_INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/include")

function(ve_collect_subsystem directory prefix)
    file(GLOB subsystem_sources CONFIGURE_DEPENDS "${directory}/*.cpp")
    file(GLOB subsystem_headers CONFIGURE_DEPENDS "${directory}/*.h" "${directory}/*.inl")
    list(SORT subsystem_sources)
    list(SORT subsystem_headers)
    set(${prefix}_SOURCES ${subsystem_sources} PARENT_SCOPE)
    set(${prefix}_HEADERS ${subsystem_headers} PARENT_SCOPE)
endfunction()

ve_collect_subsystem("${VE_SOURCE_ROOT}/core" VE_CORE)
ve_collect_subsystem("${VE_SOURCE_ROOT}/render" VE_RENDER)
file(GLOB VE_RENDER_BACKENDS_HEADERS CONFIGURE_DEPENDS
    "${VE_SOURCE_ROOT}/render/backends/*.h"
    "${VE_SOURCE_ROOT}/render/backends/*.inl"
)
list(SORT VE_RENDER_BACKENDS_HEADERS)
ve_collect_subsystem("${VE_SOURCE_ROOT}/world" VE_WORLD)
ve_collect_subsystem("${VE_SOURCE_ROOT}/network" VE_NETWORK)
ve_collect_subsystem("${VE_SOURCE_ROOT}/app" VE_APP)

function(ve_add_subsystem target folder directory)
    cmake_parse_arguments(SUBSYSTEM "" "" "SOURCES;HEADERS" ${ARGN})
    add_library(${target} STATIC ${SUBSYSTEM_SOURCES} ${SUBSYSTEM_HEADERS})
    target_include_directories(${target} PUBLIC ${directory})
    target_link_libraries(${target} PUBLIC ve_dependencies PRIVATE ve_project_options)
    ve_enable_common_pch(${target})
    set_target_properties(${target} PROPERTIES FOLDER "Engine/${folder}")
    source_group(TREE ${directory} FILES ${SUBSYSTEM_SOURCES} ${SUBSYSTEM_HEADERS})
endfunction()

function(ve_add_backend_component target folder)
    cmake_parse_arguments(COMPONENT "" "" "SOURCES;HEADERS;PUBLIC_LINKS;PRIVATE_LINKS" ${ARGN})
    add_library(${target} STATIC ${COMPONENT_SOURCES} ${COMPONENT_HEADERS})
    target_include_directories(${target} PUBLIC "${VE_SOURCE_ROOT}/render/backends")
    target_link_libraries(${target}
        PUBLIC
            ve_dependencies
            ${COMPONENT_PUBLIC_LINKS}
        PRIVATE
            ve_project_options
            ${COMPONENT_PRIVATE_LINKS}
    )
    ve_enable_common_pch(${target})
    set_target_properties(${target} PROPERTIES FOLDER "Engine/Render/${folder}")
    source_group(TREE "${VE_SOURCE_ROOT}/render/backends" FILES ${COMPONENT_SOURCES} ${COMPONENT_HEADERS})
endfunction()

function(ve_copy_msvc_asan_runtime target)
    if (NOT (VE_ENABLE_ASAN AND MSVC))
        return()
    endif()
    get_filename_component(VE_MSVC_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    file(GLOB VE_MSVC_ASAN_RUNTIME_DLLS CONFIGURE_DEPENDS
        "${VE_MSVC_BIN_DIR}/clang_rt.asan*_dynamic-x86_64.dll"
    )
    if (NOT VE_MSVC_ASAN_RUNTIME_DLLS)
        message(WARNING "MSVC AddressSanitizer runtime DLLs were not found next to ${CMAKE_CXX_COMPILER}.")
        return()
    endif()
    foreach(asan_runtime_dll IN LISTS VE_MSVC_ASAN_RUNTIME_DLLS)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${asan_runtime_dll}"
                "$<TARGET_FILE_DIR:${target}>"
            VERBATIM
        )
    endforeach()
endfunction()

ve_add_subsystem(ve_core Core "${VE_SOURCE_ROOT}/core" SOURCES ${VE_CORE_SOURCES} HEADERS ${VE_CORE_HEADERS})
ve_add_subsystem(ve_render Render "${VE_SOURCE_ROOT}/render" SOURCES ${VE_RENDER_SOURCES} HEADERS ${VE_RENDER_HEADERS})
ve_add_subsystem(ve_world World "${VE_SOURCE_ROOT}/world" SOURCES ${VE_WORLD_SOURCES} HEADERS ${VE_WORLD_HEADERS})
ve_add_subsystem(ve_network Network "${VE_SOURCE_ROOT}/network" SOURCES ${VE_NETWORK_SOURCES} HEADERS ${VE_NETWORK_HEADERS})
ve_add_subsystem(ve_app App "${VE_SOURCE_ROOT}/app" SOURCES ${VE_APP_SOURCES} HEADERS ${VE_APP_HEADERS})

set(VE_RENDER_BACKEND_WINDOW_SOURCES
    "${VE_SOURCE_ROOT}/render/backends/Window.cpp"
    "${VE_SOURCE_ROOT}/render/backends/WindowCursor.cpp"
    "${VE_SOURCE_ROOT}/render/backends/WindowDisplay.cpp"
    "${VE_SOURCE_ROOT}/render/backends/WindowInitialization.cpp"
    "${VE_SOURCE_ROOT}/render/backends/WindowRuntime.cpp"
)

set(VE_RENDER_BACKEND_OPENGL_SOURCES
    "${VE_SOURCE_ROOT}/render/backends/OpenGLCompatibilityBackend.cpp"
    "${VE_SOURCE_ROOT}/render/backends/OpenGLRenderCommandExecutor.cpp"
)

set(VE_RENDER_BACKEND_FACTORY_SOURCES
    "${VE_SOURCE_ROOT}/render/backends/DirectX12Backend.cpp"
    "${VE_SOURCE_ROOT}/render/backends/RenderBackendFactory.cpp"
)

set(VE_RENDER_BACKEND_VULKAN_SOURCES
    "${VE_SOURCE_ROOT}/render/backends/VulkanBackend.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanBackendStartup.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanChunkMeshTranslator.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanContext.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanDebugLabels.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanDemoFrameTypes.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanDevice.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanDeviceExtensions.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanFrameOrchestrator.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanFrameOrchestratorCommands.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanFrameOrchestratorGpuCommands.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanFrameOrchestratorGpuDraw.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanFrameOrchestratorImages.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanFrameOrchestratorPresentation.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanFrameOrchestratorRelease.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanFrameOrchestratorResources.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanFrameOrchestratorSoftwareCommands.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanFrameOrchestratorSoftwareDraw.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanFrameOrchestratorUpload.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanGpuChunkRenderer.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanGpuChunkRendererBuffer.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanGpuChunkRendererGraphicsPipeline.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanGpuChunkRendererImage.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanGpuChunkRendererMeshBuild.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanGpuChunkRendererMeshFaces.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanGpuChunkRendererMeshUpload.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanGpuChunkRendererPipeline.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanGpuChunkRendererPipelineLayout.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanGpuChunkRendererRecord.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanGpuChunkRendererRelease.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanGpuChunkRendererRenderPass.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanGpuChunkRendererShaders.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanGpuChunkRendererSwapchain.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanGpuChunkRendererUpload.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanHppDispatch.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanImGuiOverlay.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanMemoryAllocator.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanMinecraftDemoProfiles.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanMinecraftDemoSettings.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanPhysicalDevice.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanRenderView.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanSoftwareVoxelRasterizer.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanSoftwareVoxelRasterizerDebugOverlay.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanSoftwareVoxelRasterizerGlyphs.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanSoftwareVoxelRasterizerOutlines.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanSoftwareVoxelRasterizerRayCache.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanSoftwareVoxelRasterizerRayDda.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanSoftwareVoxelRasterizerRayShade.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanSoftwareVoxelRasterizerRayTraversal.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanSoftwareVoxelRasterizerRelease.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanSoftwareVoxelRasterizerRender.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanSoftwareVoxelRasterizerResize.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanSoftwareVoxelRasterizerSamples.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanSoftwareVoxelRasterizerSnapshot.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanSoftwareVoxelRasterizerText.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanSoftwareVoxelRasterizerTextures.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanSoftwareVoxelRasterizerTuning.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanSoftwareVoxelRasterizerTuningPanel.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanSoftwareVoxelRasterizerUpscale.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanSoftwareVoxelRasterizerWorkers.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanSurface.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanSwapchain.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanSwapchainChoices.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanSwapchainSupport.cpp"
    "${VE_SOURCE_ROOT}/render/backends/VulkanUploadBuffer.cpp"
)

ve_add_backend_component(ve_render_backend_window Window
    SOURCES ${VE_RENDER_BACKEND_WINDOW_SOURCES}
    HEADERS ${VE_RENDER_BACKENDS_HEADERS}
    PUBLIC_LINKS ve_render
    PRIVATE_LINKS glfw
)

ve_add_backend_component(ve_render_backend_opengl OpenGL
    SOURCES ${VE_RENDER_BACKEND_OPENGL_SOURCES}
    HEADERS ${VE_RENDER_BACKENDS_HEADERS}
    PUBLIC_LINKS ve_render
    PRIVATE_LINKS GLEW::GLEW
)

ve_add_backend_component(ve_render_backend_vulkan Vulkan
    SOURCES ${VE_RENDER_BACKEND_VULKAN_SOURCES}
    HEADERS ${VE_RENDER_BACKENDS_HEADERS}
    PUBLIC_LINKS ve_core ve_render ve_world volk::volk_headers Vulkan::Headers
    PRIVATE_LINKS Boost::container glfw imgui::imgui volk::volk GPUOpen::VulkanMemoryAllocator
)

ve_add_backend_component(ve_render_backend_factory Factory
    SOURCES ${VE_RENDER_BACKEND_FACTORY_SOURCES}
    HEADERS ${VE_RENDER_BACKENDS_HEADERS}
    PUBLIC_LINKS ve_render
    PRIVATE_LINKS ve_render_backend_opengl ve_render_backend_vulkan
)

add_library(ve_render_backends INTERFACE)
target_include_directories(ve_render_backends INTERFACE "${VE_SOURCE_ROOT}/render/backends")
target_link_libraries(ve_render_backends INTERFACE
    ve_render_backend_factory
    ve_render_backend_opengl
    ve_render_backend_vulkan
    ve_render_backend_window
)
set_target_properties(ve_render_backends PROPERTIES FOLDER "Engine/Render")
