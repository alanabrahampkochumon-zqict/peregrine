add_executable(Asteroids)
target_compile_features(Asteroids PRIVATE cxx_std_23)

target_sources(
        Asteroids
        PRIVATE
        Main.cpp
)

target_link_libraries(
        Asteroids PRIVATE
        Peregrine
        StrictWarnings

        imgui
        SDL3::SDL3
)

