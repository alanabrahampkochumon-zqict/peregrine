add_executable(PeregrineBenchmarks)
target_compile_features(PeregrineBenchmarks PRIVATE cxx_std_20)

target_sources(PeregrineBenchmarks PRIVATE
        SnapTo.cpp
)

target_precompile_headers(
        PeregrineBenchmarks
        PRIVATE
        <benchmark/benchmark.h>
)

target_include_directories(PeregrineBenchmarks PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(
        PeregrineBenchmarks
        PRIVATE
        benchmark::benchmark
        benchmark::benchmark_main
        StrictWarnings
)

