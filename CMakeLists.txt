cmake_minimum_required(VERSION 3.25)
project(Peregrine LANGUAGES CXX)
enable_testing()

add_library(StrictWarnings INTERFACE) # Used for linking Warning flags
add_library(ProjectASan INTERFACE) # Used for linking ASan

# Turn on solution folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(DevToolsDir "Dev Tools")

option(PMM_ASAN "Enable ASan" OFF) # Option to enable ASAN
option(PMM_STRICT_MODE "Enable Strict Warnings" OFF) # Option to enable compiler specific "Warning as Error"

option(PMM_DEMO "Enables Peregrine Demo Application")
option(PMM_DOCS "Enables Peregrine Doxygen Generation")
option(PMM_BENCHMARK "Enables Peregrine Benchmarks")
option(PMM_TESTS "Enables Peregrine Tests")

if (PMM_ASAN)
    include(cmake/ASan.cmake)
endif ()

if (PMM_STRICT_MODE)
    include(cmake/StrictMode.cmake)
endif ()

if (PMM_DOCS)
    include(cmake/Doxygen.cmake)
endif ()

# Interprocedural Optimization
include(CheckIPOSupported)
check_ipo_supported(RESULT IPO_SUPPORTED)
if(IPO_SUPPORTED)
    message(STATUS "Enabling Interprocedural Optimization")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ${IPO_SUPPORTED})
endif ()


include(cmake/ClangFormat.cmake)
include(cmake/ClangTidy.cmake)
include(cmake/Dependencies.cmake)


# Multi-processor compilation
if (MSVC AND NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Enabling Multiprocessor compilation on MSVC")
    add_compile_options("/MP")
endif ()

# Core Library
add_subdirectory(include)

if (PMM_TESTS)
    message(STATUS "Tests are enabled")
    add_subdirectory(tests)
endif ()

if (PMM_DEMO)
    message(STATUS "Demo application is enabled")
    add_subdirectory(apps/demo)
endif ()

if (PMM_BENCHMARK)
    message(STATUS "Benchmarks are enabled")
    add_subdirectory(benchmarks)
endif ()

