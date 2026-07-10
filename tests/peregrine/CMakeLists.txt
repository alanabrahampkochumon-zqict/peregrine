add_executable(PeregrineTests)

add_subdirectory(allocators)
add_subdirectory(telemetry)


target_sources(
        PeregrineTests
        PRIVATE
        ConstantTests.cpp
)

target_include_directories(
        PeregrineTests PRIVATE
        $<BUILD_INTERFACE:${ROOT_TEST_DIR}/include>
)

target_link_libraries(
        PeregrineTests PRIVATE
        GTest::gtest_main
        StrictWarnings
        Peregrine

        # Subdirectory Objects
        AllocatorTests
        TelemetryTests
)

# Preprocessor Macros
# ENABLE_PMM_TELEMETRY -> Enable telemetry testing
target_compile_definitions(PeregrineTests PRIVATE ENABLE_PMM_TELEMETRY)
target_compile_definitions(AllocatorTests PRIVATE ENABLE_PMM_TELEMETRY)
target_compile_definitions(TelemetryTests PRIVATE ENABLE_PMM_TELEMETRY)

include(GoogleTest)
gtest_discover_tests(PeregrineTests)