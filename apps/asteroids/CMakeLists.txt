add_executable(Asteroids)
target_compile_features(Asteroids PRIVATE cxx_std_23)

target_sources(
        Asteroids
        PRIVATE

        include/AsteroidGame.h
        include/AsteroidGame.cpp
        Main.cpp
)

target_link_libraries(
        Asteroids PRIVATE
        Peregrine
        StrictWarnings
        PSMath

        imgui
        SDL3::SDL3
)

