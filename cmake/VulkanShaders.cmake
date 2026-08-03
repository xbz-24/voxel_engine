set(VE_VULKAN_SHADER_OUTPUT_DIR "${PROJECT_BINARY_DIR}/generated/shaders/vulkan")
set(VE_VULKAN_SHADER_SOURCES
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_chunk.vert"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_chunk.frag"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_sky.vert"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_sky.frag"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_shadow.vert"
)
set(VE_VULKAN_SHADER_INCLUDE_SOURCES
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_aerial_perspective.glsl"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_ambient_occlusion.glsl"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_atmosphere.glsl"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_brdf.glsl"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_clouds.glsl"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_cloud_lighting.glsl"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_detail_filter.glsl"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_diffuse.glsl"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_emission.glsl"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_environment.glsl"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_grade.glsl"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_lighting.glsl"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_materials.glsl"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_math.glsl"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_noise.glsl"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_normal_detail.glsl"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_reflections.glsl"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_shadowing.glsl"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_shadow_map.glsl"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_sky.glsl"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_sky_scattering.glsl"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_specular.glsl"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_subsurface.glsl"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_surface_detail.glsl"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_surface_masks.glsl"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_tonemap.glsl"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_transform.glsl"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_vertex.glsl"
    "${PROJECT_SOURCE_DIR}/shaders/vulkan/voxel_water.glsl"
)
set(VE_VULKAN_SHADER_OUTPUTS)
foreach(shader_source IN LISTS VE_VULKAN_SHADER_SOURCES)
    get_filename_component(shader_name "${shader_source}" NAME)
    set(shader_output "${VE_VULKAN_SHADER_OUTPUT_DIR}/${shader_name}.spv")
    add_custom_command(
        OUTPUT "${shader_output}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${VE_VULKAN_SHADER_OUTPUT_DIR}"
        COMMAND "${VE_GLSLC_EXECUTABLE}" -Werror -I "${PROJECT_SOURCE_DIR}/shaders/vulkan" "${shader_source}" -o "${shader_output}"
        COMMAND "${VE_SPIRV_VAL_EXECUTABLE}" "${shader_output}"
        DEPENDS "${shader_source}" ${VE_VULKAN_SHADER_INCLUDE_SOURCES}
        COMMENT "Compiling Vulkan shader ${shader_name}"
        VERBATIM
    )
    list(APPEND VE_VULKAN_SHADER_OUTPUTS "${shader_output}")
endforeach()
add_custom_target(ve_vulkan_shaders DEPENDS ${VE_VULKAN_SHADER_OUTPUTS})
set_target_properties(ve_vulkan_shaders PROPERTIES FOLDER "Engine/Render")
add_dependencies(ve_render_backend_vulkan ve_vulkan_shaders)
target_compile_definitions(ve_render_backend_vulkan PRIVATE VE_VULKAN_SHADER_DIR="${VE_VULKAN_SHADER_OUTPUT_DIR}")
