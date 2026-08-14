cmake_minimum_required(VERSION 3.25)

if (NOT DEFINED VE_SOURCE_DIR OR VE_SOURCE_DIR STREQUAL "")
    message(FATAL_ERROR "VE_SOURCE_DIR must point at the repository root")
endif()

cmake_path(ABSOLUTE_PATH VE_SOURCE_DIR NORMALIZE OUTPUT_VARIABLE project_source_dir)
if (NOT EXISTS "${project_source_dir}/CMakeLists.txt")
    message(FATAL_ERROR "VE_SOURCE_DIR is not a Voxel Engine source tree: ${project_source_dir}")
endif()

set(policy_roots
    include/voxel
    src
    Tests
    apps
    examples
    packaging
)
set(cpp_extensions
    .cc .cpp .cppm .cxx
    .h .hh .hpp .hxx
    .ipp .ixx .tpp
)

set(authored_cpp_files)
set(forbidden_inl_files)
foreach(relative_root IN LISTS policy_roots)
    set(root "${project_source_dir}/${relative_root}")
    if (NOT IS_DIRECTORY "${root}")
        message(FATAL_ERROR "Source-policy root is missing: ${relative_root}")
    endif()

    file(GLOB_RECURSE root_files LIST_DIRECTORIES false "${root}/*")
    foreach(source_file IN LISTS root_files)
        get_filename_component(extension "${source_file}" EXT)
        string(TOLOWER "${extension}" extension)
        if (extension STREQUAL ".inl")
            list(APPEND forbidden_inl_files "${source_file}")
        elseif (extension IN_LIST cpp_extensions)
            list(APPEND authored_cpp_files "${source_file}")
        endif()
    endforeach()
endforeach()

list(REMOVE_DUPLICATES authored_cpp_files)
list(REMOVE_DUPLICATES forbidden_inl_files)
list(SORT authored_cpp_files)
list(SORT forbidden_inl_files)

function(count_physical_lines source_file output_variable)
    file(READ "${source_file}" contents)
    if (contents STREQUAL "")
        set(line_count 0)
    else()
        string(REPLACE "\r\n" "\n" contents "${contents}")
        string(REPLACE "\r" "\n" contents "${contents}")
        string(REGEX MATCHALL "\n" line_endings "${contents}")
        list(LENGTH line_endings line_count)

        string(LENGTH "${contents}" content_length)
        math(EXPR last_character_offset "${content_length} - 1")
        string(SUBSTRING "${contents}" ${last_character_offset} 1 last_character)
        if (NOT last_character STREQUAL "\n")
            math(EXPR line_count "${line_count} + 1")
        endif()
    endif()
    set(${output_variable} ${line_count} PARENT_SCOPE)
endfunction()

function(contains_nul_byte source_file output_variable)
    file(READ "${source_file}" hexadecimal_contents HEX)
    string(REGEX MATCH "^(..)*00" nul_match "${hexadecimal_contents}")
    if (nul_match)
        set(${output_variable} true PARENT_SCOPE)
    else()
        set(${output_variable} false PARENT_SCOPE)
    endif()
endfunction()

set(policy_violations)
foreach(source_file IN LISTS authored_cpp_files)
    count_physical_lines("${source_file}" line_count)
    contains_nul_byte("${source_file}" has_nul_byte)
    if (has_nul_byte)
        file(RELATIVE_PATH relative_file "${project_source_dir}" "${source_file}")
        cmake_path(CONVERT "${relative_file}" TO_CMAKE_PATH_LIST relative_file)
        list(APPEND policy_violations "binary content: ${relative_file} contains a NUL byte")
    endif()
    if (NOT line_count LESS 100)
        file(RELATIVE_PATH relative_file "${project_source_dir}" "${source_file}")
        cmake_path(CONVERT "${relative_file}" TO_CMAKE_PATH_LIST relative_file)
        list(APPEND policy_violations
            "line limit: ${relative_file} (${line_count} lines, maximum 99)"
        )
    endif()
endforeach()

foreach(source_file IN LISTS forbidden_inl_files)
    count_physical_lines("${source_file}" line_count)
    file(RELATIVE_PATH relative_file "${project_source_dir}" "${source_file}")
    cmake_path(CONVERT "${relative_file}" TO_CMAKE_PATH_LIST relative_file)
    list(APPEND policy_violations
        "forbidden extension: ${relative_file} (${line_count} lines, .inl is not allowed)"
    )
endforeach()

list(LENGTH authored_cpp_files authored_file_count)
list(LENGTH forbidden_inl_files inl_file_count)
list(LENGTH policy_violations violation_count)
list(JOIN policy_roots ", " policy_root_summary)
message(STATUS
    "Source policy scanned ${authored_file_count} authored C++ files and "
    "${inl_file_count} .inl files in: ${policy_root_summary}"
)

if (policy_violations)
    list(SORT policy_violations)
    foreach(violation IN LISTS policy_violations)
        message(STATUS "SOURCE POLICY VIOLATION: ${violation}")
    endforeach()
    message(FATAL_ERROR "Source policy failed with ${violation_count} violation(s)")
endif()

message(STATUS
    "Source policy passed: every authored C++ file has fewer than 100 lines and no NUL bytes"
)
