if (VE_BUILD_RUNTIME_SMOKE_TESTS)
    enable_testing()
    add_test(NAME voxel_demo_vulkan_runtime_smoke
        COMMAND $<TARGET_FILE:voxel_demo>
            --graphics-api vulkan
            --smoke-frames ${VE_RUNTIME_SMOKE_FRAMES}
    )
    set_tests_properties(voxel_demo_vulkan_runtime_smoke PROPERTIES
        LABELS "runtime;smoke;vulkan"
    )
    add_test(NAME voxel_demo_opengl_runtime_smoke
        COMMAND $<TARGET_FILE:voxel_demo>
            --graphics-api opengl
            --smoke-frames ${VE_RUNTIME_SMOKE_FRAMES}
    )
    set_tests_properties(voxel_demo_opengl_runtime_smoke PROPERTIES
        LABELS "runtime;smoke;opengl"
    )
endif()
