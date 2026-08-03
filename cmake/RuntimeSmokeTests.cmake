if (VE_BUILD_RUNTIME_SMOKE_TESTS)
    enable_testing()
    add_test(NAME voxel_demo_runtime_smoke
        COMMAND $<TARGET_FILE:voxel_demo> --smoke-frames ${VE_RUNTIME_SMOKE_FRAMES}
    )
endif()
