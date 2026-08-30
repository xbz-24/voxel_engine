set(VE_SOURCE_POLICY_SCRIPT "${PROJECT_SOURCE_DIR}/cmake/VerifySourcePolicy.cmake")
set(VE_SOURCE_POLICY_COMMAND
    "${CMAKE_COMMAND}"
    "-DVE_SOURCE_DIR:PATH=${PROJECT_SOURCE_DIR}"
    -P "${VE_SOURCE_POLICY_SCRIPT}"
)

add_custom_target(verify_source_policy
    COMMAND ${VE_SOURCE_POLICY_COMMAND}
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    COMMENT "Checking authored C++ source size and forbidden .inl files"
    VERBATIM
)
set_target_properties(verify_source_policy PROPERTIES FOLDER "Quality")

enable_testing()
add_test(NAME source_policy COMMAND ${VE_SOURCE_POLICY_COMMAND})
set_tests_properties(source_policy PROPERTIES LABELS "quality;source-policy")
