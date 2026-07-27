add_library(AllocatorTests OBJECT)

target_sources(
        AllocatorTests
        PRIVATE
        ArenaTests.cpp
        stack/LooseStackTests.cpp
        stack/StrictStackTests.cpp
        stack/StackMemoryManagementTests.cpp
        stack/TelemetryIntegrationTests.cpp
        TempArenaTests.cpp
)

target_include_directories(
        AllocatorTests PRIVATE
        ${ROOT_TEST_DIR}/include
)

target_link_libraries(
        AllocatorTests PRIVATE
        PeregrineTestSetup
)
