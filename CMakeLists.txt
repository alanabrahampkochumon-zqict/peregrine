cmake_minimum_required(VERSION 3.25)
project(Peregrine LANGUAGES CXX)
enable_testing()

add_library(StrictWarnings INTERFACE) # Used for linking Warning flags
add_library(ProjectASan INTERFACE) # Used for linking ASan

# Turn on solution folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(DevToolsDir "Dev Tools")


# Flags for keeping the library lightweight when a consumer imports it
option(ENABLE_PMM_BENCHMARK "Enables PMM Benchmark Suite")
option(ENABLE_PMM_DOCS "Enables PMM Documentation Generation")


# Clang Formatter
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


# Clang Tidy
find_program(CLANG_TIDY_EXE NAMES clang-tidy)
if (CLANG_TIDY_EXE)
    # Create a custom target that you manually trigger
    add_custom_target(
            lint
            COMMAND ${CMAKE_COMMAND} -E echo "Running Clang-Tidy on MathLib..."
            COMMAND run-clang-tidy -p ${CMAKE_BINARY_DIR} -header-filter=.*
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Running static analysis..."
    )
    set_target_properties(lint PROPERTIES FOLDER ${DevToolsDir})
else ()
    message(WARNING "clang-tidy not found")
endif ()


# Doxygen
if (ENABLE_FGM_DOCS)
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
    else ()
        message(WARN "Doxygen not found! Skipping documentation generation.")
    endif ()
endif ()


option(ENABLE_STRICT "Enable Strict Warnings" OFF) # Option to enable compiler specific "Warning as Error"
option(ASAN "Enable ASan" OFF) # Option to enable ASAN


# Enables Strict Warnings and Debug-only ASan
if (ENABLE_STRICT)
    message(STATUS "Configuring Strict Warnings")
    if (MSVC AND NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(StrictWarnings INTERFACE
                $<$<COMPILE_LANGUAGE:CXX>:/WX;/W4;/permissive-;/fp:strict;/wd4723>
        )
    elseif (MSVC AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(StrictWarnings INTERFACE
                $<$<COMPILE_LANGUAGE:CXX>:-Wall;-Wextra;-Wpedantic;-Werror>
                $<$<CXX_COMPILER_ID:Clang>:
                # Backwards compatibility warning suppression
                /clang:-Wno-c++98-compat;
                /clang:-Wno-c++98-compat-pedantic;
                /clang:-Wno-pre-c++14-compat;
                /clang:-Wno-pre-c++17-compat;
                /clang:-Wno-c++20-compat;

                /clang:-Wno-global-constructors; # Gtest
                /clang:-Wno-covered-switch-default;
                /clang:-Wno-switch-default; # Gtest
                /clang:-Wno-unique-object-duplication;
                /clang:-Wno-header-hygiene;
                /clang:-Wno-float-equal;
                /clang:-Wno-unsafe-buffer-usage;
                /clang:-Wno-padded; # Gtest
                /clang:-Wno-implicit-int-float-conversion;
                /clang:-Wno-double-promotion;
                /clang:-Wno-implicit-int-conversion-on-negation
                /clang:-Wno-reserved-macro-identifier; # Required by cl-clang on CLion due to macro conflicts
                /clang:-Wno-unused-macros; # Required by cl-clang on CLion due to macro conflicts
                >
        )
        set(CMAKE_DISABLE_PRECOMPILE_HEADERS ON)
    else ()
        target_compile_options(StrictWarnings INTERFACE
                $<$<COMPILE_LANGUAGE:CXX>:-Wall;-Wextra;-Werror;-pedantic-errors>
                $<$<CXX_COMPILER_ID:Clang>:-Wno-c++98-compat;-Wno-c++98-compat-pedantic;-Wno-pre-c++14-compat;-Wno-pre-c++17-compat;-Wno-c++20-compat>
                $<$<CXX_COMPILER_ID:AppleClang>:-Wno-c++98-compat;-Wno-c++98-compat-pedantic;-Wno-pre-c++14-compat;-Wno-pre-c++17-compat;-Wno-c++20-compat>
        )
    endif ()
endif ()


# Enables Address Sanitizer for all the projects globally.
if (ASAN)
    message(STATUS "Turning on Address Sanitizer for Debug Builds")
    if (MSVC AND NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(StrictWarnings INTERFACE $<$<CONFIG:Debug>:/fsanitize=address;/Zi>)
        target_link_options(StrictWarnings INTERFACE $<$<CONFIG:Debug>:/INCREMENTAL:NO>)
    else ()
        target_compile_options(StrictWarnings INTERFACE $<$<CONFIG:Debug>:-fsanitize=address;-fno-omit-frame-pointer>)
        target_link_options(StrictWarnings INTERFACE $<$<CONFIG:Debug>:-fsanitize=address>)
    endif ()
endif ()

# Multi-processor compilation
if (MSVC AND NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Enabling Multiprocessor compilation on MSVC")
    add_compile_options("/MP")
endif ()

include(cmake/dependencies.cmake)

add_subdirectory(src)
add_subdirectory(tests)


if (ENABLE_PMM_BENCHMARK)
    message("Benchmarks are enabled")
    add_subdirectory(benchmark)
    target_compile_features(BenchmarkSuite PRIVATE cxx_std_20)
endif ()

