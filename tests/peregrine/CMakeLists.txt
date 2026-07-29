add_executable(PeregrineTests $<TARGET_OBJECTS:AllocatorTests> $<TARGET_OBJECTS:TelemetryTests>)

add_library(PeregrineTestSetup INTERFACE)
target_link_libraries(PeregrineTestSetup INTERFACE
        GTest::gtest_main
        StrictWarnings
        Peregrine
)
# Enables GTest's Friend tests
# Unless the consuming application has GTest imported, DON'T enable this!!
target_compile_definitions(PeregrineTestSetup INTERFACE ENABLE_PMM_TESTS ENABLE_PMM_TELEMETRY)

add_subdirectory(allocators)
add_subdirectory(telemetry)
add_subdirectory(utils)


target_sources(
        PeregrineTests
        PRIVATE
        utils/ConstantTests.cpp
)

target_include_directories(
        PeregrineTests PRIVATE
        $<BUILD_INTERFACE:${ROOT_TEST_DIR}/include>
)

target_link_libraries(
        PeregrineTests PRIVATE
        PeregrineTestSetup

        # Subdirectory Objects
        AllocatorTests
        TelemetryTests
        UtilsTests
)


include(GoogleTest)
gtest_discover_tests(PeregrineTests)

set_target_properties(AllocatorTests TelemetryTests PROPERTIES FOLDER "Test Intermediates/Peregrine")
