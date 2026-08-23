add_executable(
        Asteroids

        components/Actor.h

        game/AsteroidGame.h
        game/AsteroidGame.cpp
        Main.cpp
        components/GameObject.h
        components/Component.h
        components/Actor.cpp

        graphics/Texture.h
        graphics/Texture.cpp
        components/SpriteComponent.cpp
        components/SpriteComponent.h
        components/AnimSpriteComponent.cpp
        components/AnimSpriteComponent.h
)

target_compile_features(Asteroids PRIVATE cxx_std_20)

target_include_directories(Asteroids PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
        Asteroids PRIVATE
        Peregrine
        StrictWarnings
        PSMath

        imgui
        SDL3::SDL3
        SDL3_image::SDL3_image
)

