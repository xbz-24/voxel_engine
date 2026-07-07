if (MSVC)
    target_compile_options(ve_project_options INTERFACE
        /MP
        /permissive-
        /Zc:__cplusplus
        /Zc:preprocessor
        /Zc:throwingNew
        /Zc:inline
        /EHsc
        /utf-8
        /diagnostics:caret
        /external:anglebrackets
        /external:W0
        /sdl
    )
    if (VE_USE_LATEST_CXX)
        target_compile_options(ve_project_options INTERFACE /std:c++latest)
    endif()
    if (VE_ENABLE_STRICT_WARNINGS)
        target_compile_options(ve_project_options INTERFACE
            /Wall
            /wd4514
            /wd4625
            /wd4626
            /wd4710
            /wd4711
            /wd4820
            /wd5026
            /wd5027
            /wd5045
            /w14242
            /w14254
            /w14263
            /w14265
            /w14287
            /we4289
            /w14296
            /w14311
            /w14545
            /w14546
            /w14547
            /w14549
            /w14555
            /w14619
            /w14640
            /w14826
            /w14905
            /w14906
            /w14928
        )
    endif()
    if (VE_WARNINGS_AS_ERRORS)
        target_compile_options(ve_project_options INTERFACE /WX)
    endif()
else()
    if (VE_USE_LATEST_CXX)
        if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 18)
            target_compile_options(ve_project_options INTERFACE -std=c++2c)
        elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 14)
            target_compile_options(ve_project_options INTERFACE -std=c++2c)
        endif()
    endif()
    if (VE_ENABLE_STRICT_WARNINGS)
        target_compile_options(ve_project_options INTERFACE
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            -Wsign-conversion
            -Wshadow
            -Wnon-virtual-dtor
            -Wold-style-cast
            -Woverloaded-virtual
            -Wnull-dereference
            -Wdouble-promotion
            -Wformat=2
        )
    endif()
    if (VE_WARNINGS_AS_ERRORS)
        target_compile_options(ve_project_options INTERFACE -Werror)
    endif()
endif()
