cmake_minimum_required(VERSION 3.25)
project(GenericProject LANGUAGES CXX)
enable_testing()

add_library(StrictWarnings INTERFACE) # Used for linking Warning flags
add_library(ProjectASan INTERFACE) # Used for linking ASan

# Turn on solution folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(DevToolsDir "Dev Tools")

# Clang Formatter
find_program(CLANG_FORMATTER NAMES clang-format)

if(CLANG_FORMATTER)
    file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h *.cxx *.hxx *.hpp *.cc *.hh *.tpp *.inl)

    add_custom_target(format
        COMMAND ${CLANG_FORMATTER} -i -style=file ${ALL_SOURCE_FILES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Formatting source files..."
    )
    set_target_properties(format PROPERTIES FOLDER ${DevToolsDir})
else()
    message(WARNING "clang-format not found")
endif()


# Clang Tidy
find_program(CLANG_TIDY_EXE NAMES clang-tidy)
if(CLANG_TIDY_EXE)
    # Create a custom target that you manually trigger
    add_custom_target(
        lint
        COMMAND ${CMAKE_COMMAND} -E echo "Running Clang-Tidy on MathLib..."
        COMMAND run-clang-tidy -p ${CMAKE_BINARY_DIR} -header-filter=.*
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running static analysis..."
    )
    set_target_properties(lint PROPERTIES FOLDER ${DevToolsDir})
else()
    message(WARNING "clang-tidy not found")
endif()


# Doxygen
find_package(Doxygen)
if (DOXYGEN_FOUND)
    message(STATUS "Doxygen found!")
    doxygen_add_docs(
        docs
        ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating documentation with doxygen..."
        CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
        ALL
    )
    set_target_properties(docs PROPERTIES FOLDER ${DevToolsDir})
else()
    message(WARN "Doxygen not found! Skipping documentation generation.")
endif()



option(ENABLE_STRICT "Enable Strict Warnings" OFF) # Option to enable compiler specific "Warning as Error"
option(ASAN "Enable ASan" OFF) # Option to enable ASAN

# Enables Strict Warnings and Debug-only ASan
if(ENABLE_STRICT)
    message(STATUS "Configuring Strict Warnings")
    if(MSVC AND NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(StrictWarnings INTERFACE
            $<$<COMPILE_LANGUAGE:CXX>:/WX;/W4;/permissive-;/fp:strict>
        )
    else()
        target_compile_options(StrictWarnings INTERFACE
            $<$<COMPILE_LANGUAGE:CXX>:-Wall;-Wextra;-Werror;-pedantic-errors>
            $<$<CXX_COMPILER_ID:Clang>:-Wno-c++98-compat;-Wno-c++98-compat-pedantic;-Wno-pre-c++14-compat;-Wno-pre-c++17-compat;-Wno-c++20-compat>
            $<$<CXX_COMPILER_ID:AppleClang>:-Wno-c++98-compat;-Wno-c++98-compat-pedantic;-Wno-pre-c++14-compat;-Wno-pre-c++17-compat;-Wno-c++20-compat>
        )
    endif()
endif()


# Enables Address Sanitizer for all the projects globally.
if(ASAN)
    message(STATUS "Turning on Address Sanitizer for Debug Builds")
    if(MSVC AND NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(StrictWarnings INTERFACE $<$<CONFIG:Debug>:/fsanitize=address;/Zi>)
        target_link_options(StrictWarnings INTERFACE $<$<CONFIG:Debug>:/INCREMENTAL:NO>)
    else()
        target_compile_options(StrictWarnings INTERFACE $<$<CONFIG:Debug>:-fsanitize=address;-fno-omit-frame-pointer>)
        target_link_options(StrictWarnings INTERFACE $<$<CONFIG:Debug>:-fsanitize=address>)
    endif()
endif()

# Multi-processor compilation
if(MSVC AND NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Enabling Multiprocessor compilation on MSVC")
    add_compile_options("/MP")
endif()

include(dependencies.cmake)

add_subdirectory(test-suite)
add_subdirectory(benchmark-suite)

# Link ProjectStrictWarnings to enable strict mode.
target_link_libraries(TestSuite PRIVATE StrictWarnings)
target_link_libraries(BenchmarkSuite PRIVATE StrictWarnings)


set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT TestSuite)