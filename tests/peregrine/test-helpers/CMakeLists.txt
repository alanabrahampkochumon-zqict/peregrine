add_library(TestHelpers STATIC)

target_compile_features(TestHelpers PUBLIC cxx_std_20)

target_sources(
        TestHelpers PUBLIC
        FILE_SET CXX_MODULES
        FILES
        Mocks.cppm
)