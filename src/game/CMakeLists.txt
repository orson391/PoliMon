add_library(game STATIC
    Game.cpp
    entities/Player.cpp
    world/Tileset.cpp
    world/TileMap.cpp
    world/CollisionLayer.cpp
    world/ObjectLayer.cpp
    world/EntityLayer.cpp
    world/MapLoader.cpp
    world/World.cpp
    world/ColorTileMap.cpp

    graphics/TextureManager.cpp
)

target_include_directories(game PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(game PUBLIC core engine SDL3::SDL3 SDL3_image::SDL3_image)

target_compile_features(game PUBLIC cxx_std_17)
