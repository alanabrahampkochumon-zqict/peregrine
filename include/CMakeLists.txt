
add_subdirectory(peregrine)


if (ENABLE_PMM_DEMO)
    message("Demo application is enabled")
    add_subdirectory(demo)
    target_compile_features(Demo PRIVATE cxx_std_20)
endif ()

