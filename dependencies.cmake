include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG 52eb8108c5bdec04579160ae17225d66034bd723 # release-1.17.0
    SYSTEM
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/vendors/googletest"
)

FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.9.5
    SYSTEM
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/vendors/googlebenchmark"
)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE) # Disbles google benchmark from creating its own test suite

FetchContent_MakeAvailable(googletest)
FetchContent_MakeAvailable(googlebenchmark)

# Group the projects into a single folder
set_target_properties(gtest gtest_main gmock gmock_main PROPERTIES FOLDER "Google Test")
set_target_properties(benchmark benchmark_main PROPERTIES FOLDER "Google Benchmark")