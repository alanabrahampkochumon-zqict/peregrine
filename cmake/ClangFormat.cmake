include_guard()

find_program(CLANG_FORMATTER NAMES clang-format)

if (CLANG_FORMATTER)
    file(GLOB_RECURSE ALL_SOURCE_FILES CONFIGURE_DEPENDS
            "${CMAKE_SOURCE_DIR}/src/*.cpp" "${CMAKE_SOURCE_DIR}/src/*.h" "${CMAKE_SOURCE_DIR}/src/*.tpp" "${CMAKE_SOURCE_DIR}/src/*.inl"
            "${CMAKE_SOURCE_DIR}/tests/*.cpp" "${CMAKE_SOURCE_DIR}/tests/*.h" "${CMAKE_SOURCE_DIR}/tests/*.tpp" "${CMAKE_SOURCE_DIR}/tests/*.inl"
            "${CMAKE_SOURCE_DIR}/benchmarks/*.cpp" "${CMAKE_SOURCE_DIR}/benchmarks/*.h" "${CMAKE_SOURCE_DIR}/benchmarks/*.tpp" "${CMAKE_SOURCE_DIR}/benchmarks/*.inl"
    )
    # Only add the target if files were actually found to avoid the stdin error
    if (ALL_SOURCE_FILES)
        add_custom_target(format
                COMMAND "${CLANG_FORMATTER}" -i -style=file ${ALL_SOURCE_FILES}
                WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
                COMMENT "Formatting source files..."
                VERBATIM
        )
        set_target_properties(format PROPERTIES FOLDER ${DevToolsDir})
    else ()
        message(STATUS "No source files found for formatting.")
    endif ()

else ()
    message(WARNING "clang-format not found")
endif ()