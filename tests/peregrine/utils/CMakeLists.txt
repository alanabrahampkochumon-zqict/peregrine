add_library(
        UtilsTests OBJECT
)

target_sources(
        UtilsTests PRIVATE
        ConstantTests.cpp
        HelpersTest.cpp
)

target_include_directories(
        UtilsTests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)


target_link_libraries(
        UtilsTests PRIVATE
        PeregrineTestSetup
)