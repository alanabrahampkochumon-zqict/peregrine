include_guard()
include(FetchContent)

set(VENDORS_DIR "Vendors")

# Make SDL3 a static library
# https://wiki.libsdl.org/SDL3/README-cmake
set(SDL_SHARED OFF CACHE BOOL "Build shared library" FORCE)
set(SDL_STATIC ON CACHE BOOL "Build static library" FORCE)
# Build SDL3_image as a static lib
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build SDL3_image shared" FORCE)
# Enable these if necessary as some of them have extra dependencies like nasm and perl
set(SDLIMAGE_AVIF OFF CACHE BOOL "Disable AVIF support" FORCE)
set(SDLIMAGE_WEBP OFF CACHE BOOL "Disable WebP support" FORCE)
set(SDLIMAGE_TIF OFF CACHE BOOL "Disable TIFF support" FORCE)
set(SDLIMAGE_JXL OFF CACHE BOOL "Disable JPEG-XL support" FORCE)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG 52eb8108c5bdec04579160ae17225d66034bd723 # release-1.17.0
        SYSTEM
)

FetchContent_Declare(
        google_benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.9.5
        SYSTEM
)

FetchContent_Declare(
        sdl3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG d9d5536704d585616d4db3c8ba3c4ff6fc2757e1
        OVERRIDE_FIND_PACKAGE
        SYSTEM
)

FetchContent_Declare(
        sdl3_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG release-3.4.4
        SYSTEM
)

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG f1cc2ae15e53a861a874c3034aae6798fde194ab
        SYSTEM
)


set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE) # Disables google benchmark from creating its own test suite

if (PMM_TESTS)
    FetchContent_MakeAvailable(googletest)
    set_target_properties(gtest gtest_main gmock gmock_main PROPERTIES FOLDER "Vendors/Google/GTest")
endif ()

if (PMM_BENCHMARK)
    FetchContent_MakeAvailable(google_benchmark)
    set_target_properties(benchmark benchmark_main PROPERTIES FOLDER "Vendors/Google/Benchmark")
endif ()

if (PMM_DEMO)
    FetchContent_MakeAvailable(sdl3)
    set(SDL3_DIR "${sdl3_BINARY_DIR}" CACHE PATH "Path to SDL3 build" FORCE)
    FetchContent_MakeAvailable(sdl3_image)
    FetchContent_MakeAvailable(imgui)
    # Since ImGui doesn't have cmake support we need to create a custom target
    add_library(imgui STATIC
            ${imgui_SOURCE_DIR}/imgui.cpp
            ${imgui_SOURCE_DIR}/imgui_demo.cpp
            ${imgui_SOURCE_DIR}/imgui_draw.cpp
            ${imgui_SOURCE_DIR}/imgui_tables.cpp
            ${imgui_SOURCE_DIR}/imgui_widgets.cpp
            # SDL3 backend
            ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
            ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.cpp
    )

    target_include_directories(imgui PUBLIC
            ${imgui_SOURCE_DIR}
            ${imgui_SOURCE_DIR}/backends
    )

    target_link_libraries(imgui PUBLIC SDL3::SDL3)

    set_target_properties(imgui PROPERTIES FOLDER "${VENDORS_DIR}/DearImGui")
    set_target_properties(SDL3-static SDL3_test PROPERTIES FOLDER "${VENDORS_DIR}/SDL3")
endif ()