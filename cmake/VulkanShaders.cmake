set(VE_VULKAN_SHADER_OUTPUT_DIR "${PROJECT_BINARY_DIR}/generated/shaders/vulkan")
set(VE_VULKAN_SHADER_SOURCES
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_chunk.vert"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_chunk.frag"
)
set(VE_VULKAN_SHADER_OUTPUTS)
foreach(shader_source IN LISTS VE_VULKAN_SHADER_SOURCES)
    get_filename_component(shader_name "${shader_source}" NAME)
    set(shader_output "${VE_VULKAN_SHADER_OUTPUT_DIR}/${shader_name}.spv")
    add_custom_command(
        OUTPUT "${shader_output}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${VE_VULKAN_SHADER_OUTPUT_DIR}"
        COMMAND "${VE_GLSLC_EXECUTABLE}" "${shader_source}" -o "${shader_output}"
        DEPENDS "${shader_source}"
        COMMENT "Compiling Vulkan shader ${shader_name}"
        VERBATIM
    )
    list(APPEND VE_VULKAN_SHADER_OUTPUTS "${shader_output}")
endforeach()
add_custom_target(ve_vulkan_shaders DEPENDS ${VE_VULKAN_SHADER_OUTPUTS})
set_target_properties(ve_vulkan_shaders PROPERTIES FOLDER "Engine/Render")
add_dependencies(ve_render_backends ve_vulkan_shaders)
target_compile_definitions(ve_render_backends PRIVATE VE_VULKAN_SHADER_DIR="${VE_VULKAN_SHADER_OUTPUT_DIR}")
