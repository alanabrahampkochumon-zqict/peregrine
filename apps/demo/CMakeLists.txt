add_executable(Demo)
target_compile_features(Demo PRIVATE cxx_std_23)

target_sources(
        Demo
        PRIVATE
        Main.cpp
)

target_link_libraries(
        Demo PRIVATE
        Peregrine
        StrictWarnings

        imgui
        SDL3::SDL3
)

