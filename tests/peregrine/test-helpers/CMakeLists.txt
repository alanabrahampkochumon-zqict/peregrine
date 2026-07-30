add_library(TestHelpers STATIC)

target_sources(
        TestHelpers PUBLIC
        FILE_SET FILE_TYPE CXX_MODULES
        FILES
        Mocks.cppm
)