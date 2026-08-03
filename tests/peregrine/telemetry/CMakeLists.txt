add_library(
        TelemetryTests OBJECT
)

target_sources(
        TelemetryTests PRIVATE
        ArenaTelemetryTests.cpp
        StackTelemetryTests.cpp
        PoolTelemetryTests.cpp
)

target_include_directories(
        TelemetryTests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)


target_link_libraries(
        TelemetryTests PRIVATE
        PeregrineTestSetup
)