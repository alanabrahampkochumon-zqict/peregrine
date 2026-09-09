add_library(
        UtilsTests OBJECT
        BitTests.cpp
)

target_compile_features(UtilsTests PUBLIC cxx_std_20)

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