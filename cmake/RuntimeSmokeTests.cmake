if (VE_BUILD_RUNTIME_SMOKE_TESTS)
    enable_testing()
    add_test(NAME voxel_demo_vulkan_runtime_smoke
        COMMAND $<TARGET_FILE:voxel_demo>
            --graphics-api vulkan
            --smoke-frames ${VE_RUNTIME_SMOKE_FRAMES}
    )
    set_tests_properties(voxel_demo_vulkan_runtime_smoke PROPERTIES
        LABELS "runtime;smoke;vulkan"
        TIMEOUT 60
    )
    add_test(NAME voxel_demo_vulkan_explicit_runtime_layout_smoke
        COMMAND $<TARGET_FILE:voxel_demo>
            --graphics-api vulkan
            --asset-directory "${PROJECT_SOURCE_DIR}/assets"
            --vulkan-shader-directory "${VE_VULKAN_SHADER_OUTPUT_DIR}"
            --smoke-frames ${VE_RUNTIME_SMOKE_FRAMES}
    )
    set_tests_properties(voxel_demo_vulkan_explicit_runtime_layout_smoke PROPERTIES
        LABELS "runtime;layout;smoke;vulkan"
        TIMEOUT 60
    )
    add_test(NAME voxel_demo_opengl_runtime_smoke
        COMMAND $<TARGET_FILE:voxel_demo>
            --graphics-api opengl
            --smoke-frames ${VE_RUNTIME_SMOKE_FRAMES}
    )
    set_tests_properties(voxel_demo_opengl_runtime_smoke PROPERTIES
        LABELS "runtime;smoke;opengl"
        TIMEOUT 60
    )
endif()
