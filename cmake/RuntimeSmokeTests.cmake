if (VE_BUILD_RUNTIME_SMOKE_TESTS)
    enable_testing()
    set(VE_RUNTIME_SMOKE_DEMOS desert aqua sponza)
    foreach(runtime_smoke_demo IN LISTS VE_RUNTIME_SMOKE_DEMOS)
        add_test(NAME voxel_engine_runtime_smoke_${runtime_smoke_demo}
            COMMAND $<TARGET_FILE:voxel_engine> --demo ${runtime_smoke_demo} --smoke-frames ${VE_RUNTIME_SMOKE_FRAMES}
        )
    endforeach()
endif()
