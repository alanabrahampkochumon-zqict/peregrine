add_executable(
        Asteroids

        actors/Actor.h
        actors/Actor.cpp
        actors/Asteroid.cpp
        actors/Asteroid.h
        actors/Ship.cpp
        actors/Ship.h
        actors/Bullet.cpp
        actors/Bullet.h

        game/AsteroidGame.h
        game/AsteroidGame.cpp
        Main.cpp

        components/GameObject.h
        components/Component.h
        components/SpriteComponent.cpp
        components/SpriteComponent.h
        components/AnimSpriteComponent.cpp
        components/AnimSpriteComponent.h
        components/BGSpriteComponent.cpp
        components/BGSpriteComponent.h
        components/MoveComponent.cpp
        components/MoveComponent.h
        components/SpriteSheetComponent.cpp
        components/SpriteSheetComponent.h
        components/InputComponent.cpp
        components/InputComponent.h
        components/CircleComponent.cpp
        components/CircleComponent.h

        graphics/Texture.h
        graphics/Texture.tpp
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

add_custom_command(
        TARGET Asteroids
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
        copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:Asteroids>/assets"
        COMMENT "Copying assets to build directory..."
)
