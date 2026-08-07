################################################
# Normal Tests
add_library(AllocatorTests OBJECT)

target_sources(
        AllocatorTests
        PRIVATE
        #        ArenaTests.cpp
        arena/ManagedArenaTests.cpp
        stack/LooseStackTests.cpp
        stack/StrictStackTests.cpp
        stack/StackMemoryManagementTests.cpp
        stack/TelemetryIntegrationTests.cpp
        pool/AllocatorTests.cpp
        pool/TelemetryIntegrationTests.cpp
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


################################################
# Death Tests

add_library(AllocatorDeathTests OBJECT)

target_sources(
        AllocatorDeathTests
        PRIVATE
        pool/DeathTests.cpp
        stack/DeathTests.cpp
)

target_include_directories(
        AllocatorDeathTests PRIVATE
        ${ROOT_TEST_DIR}/include
)

target_link_libraries(
        AllocatorDeathTests PRIVATE
        PeregrineTestSetup
)