include_guard()

find_program(CLANG_TIDY_EXE NAMES clang-tidy)
if (CLANG_TIDY_EXE)
    # Create a custom target that you manually trigger
    add_custom_target(
            ${PROJECT_NAME}-Tidy
            COMMAND ${CMAKE_COMMAND} -E echo "Running Clang-Tidy on MathLib..."
            COMMAND run-clang-tidy -p ${CMAKE_BINARY_DIR} -header-filter=.*
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Running static analysis..."
    )
    set_target_properties(${PROJECT_NAME}-Tidy PROPERTIES FOLDER ${DevToolsDir})
else ()
    message(WARNING "clang-tidy not found")
endif ()