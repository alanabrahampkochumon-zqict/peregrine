add_executable(Pong)
target_compile_features(Pong PRIVATE cxx_std_23)

target_sources(
        Pong
        PRIVATE
        Main.cpp

        include/Pong.cpp
        include/Pong.h

        # TODO: Group into a math lib
        include/Vec2.h
)

target_link_libraries(
        Pong PRIVATE
        Peregrine
        StrictWarnings

        imgui
        SDL3::SDL3
)

