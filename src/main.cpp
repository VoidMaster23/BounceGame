#include "raylib.h"
#include <iostream>

#define RAYLIB_TILESON_IMPLEMENTATION
#include "raylib-tileson.hpp"

typedef struct
{
    Vector2 position{};
    Vector2 speed{};
    Vector2 size{};
    float jump_height;
} Player;

// BoiundingBox shoudl probably be scoped to a namespace
typedef struct
{
    int left;
    int right;
    int top;
    int bottom;
    int height;
    int width;
} BoundingBoxCustom;

BoundingBoxCustom determine_ground_bounding_box(Map *map)
{
    // Define the bounding box coordinates
    int left = std::numeric_limits<int>::max();
    int top = std::numeric_limits<int>::max();
    int right = std::numeric_limits<int>::min();
    int bottom = std::numeric_limits<int>::min();
    int tileWidth{0};
    int tileHeight{0};

    // Adjust this layer name or condition as per your map design
    std::string groundLayerName{"Ground"};

    // Iterate through the map layers to find the ground layer
    for (auto &layer : map->data->map->getLayers())
    {
        if (layer.getType() == tson::LayerType::TileLayer && layer.getName() == groundLayerName)
        {

            const auto tile{layer.getTileObjects().begin()};
            const std::tuple<int, int> position{tile->first};
            const tson::TileObject tileObject{tile->second};

            if (const_cast<tson::TileObject &>(tileObject).getTile())
            {
                // Extract the tuple into x and y
                int tilePosX = std::get<0>(position);
                int tilePosY = std::get<1>(position);

                // get width and height of the tile
                tileWidth = (map->data->map->getTileSize().x);
                tileHeight = (map->data->map->getTileSize().y);

                // Calculate tile's world position
                int worldX = tilePosX * tileWidth;
                int worldY = tilePosY * tileHeight;

                // Update the bounding box coordinates
                left = std::min(left, worldX);
                top = std::min(top, worldY);
                right = std::max(right, worldX + tileWidth);
                bottom = std::max(bottom, worldY + tileHeight);
            }
        }
    }

    // Output the bounding box
    // TODO: clean this up
    if (left <= right && top <= bottom)
    {
        std::cout << "Ground Bounding Box:" << std::endl;
        std::cout << "Left: " << left << ", Right: " << right << std::endl;
        std::cout << "Top: " << top << ", bottom: " << bottom << std::endl;
        std::cout << "Width: " << tileWidth << ", Height: " << tileHeight << std::endl;
    }
    else
    {
        std::cout << "No ground tiles found." << std::endl;
    }

    return {
        .left = left,
        .right = right,
        .top = top,
        .bottom = bottom,
        .width = tileWidth,
        .height = tileHeight};
}

/*
Initializes the window as well  as basic start parameters for the game
*/
void init_screen()
{
    const int screenWidth{1920};
    const int screenHeight{1080};

    SetConfigFlags(FLAG_MSAA_4X_HINT); // Anti Aliasing to Smooth Edges
    InitWindow(screenWidth, screenHeight, "Bounce");

    SetTargetFPS(120); // TODO: properly use delta time for jumps
}

int main(void)
{

    // ========== Init the screen ==================
    init_screen();

    bool isPaused{false};
    int framesCounter{0};

    Map map = LoadTiled(ASSETS_PATH "maps/BounceMap.json");

    if (!IsTiledReady(map))
    {
        std::cerr << "Could not load the map, maybe check the filepath" << std::endl;
        return 1;
    }

    // ======== Find the Floor ============================

    BoundingBoxCustom ground{determine_ground_bounding_box(&map)};

    // ============Init the player ====================
    Player player{
        .size = {ground.width * 0.5f, ground.height * 0.9f},
        .position = {ground.width * 2.0f, ground.top - ground.height * 0.9f},
        .speed = {ground.width * 0.5f, ground.width * 0.2f},
        .jump_height = (static_cast<float>(GetScreenHeight()) - 4.0f * ground.height)};
    //===================================================

    bool isRising{false};
    bool isFalling{false};
    bool isOnGround{(player.position.y == ground.top - player.size.y)};

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {

        // ================== Player Input ====================
        if ((IsKeyDown(KEY_UP) || IsKeyDown(KEY_SPACE)) && isOnGround)
        {
            isRising = true;
            isOnGround = false;
        }

        // ============== Sate Update ========================
        float delta_time = GetFrameTime();

        // Checks to see if we haven't reached the max jump height
        // TODO: refactor the check for max height
        if (isRising && (player.position.y > player.jump_height))
        {
            player.position.y -= player.speed.y * (20.0f * delta_time);
        }

        if (isRising && (player.position.y <= player.jump_height))
        {
            isRising = false;
            isFalling = true;
        }

        if (isFalling && (player.position.y < ground.top - player.size.y))
        {
            player.position.y += player.speed.y * (20.0f * delta_time);
        }

        // should probably check the bottom of the balll
        if (isFalling && (player.position.y >= ground.top - player.size.y))
        {
            isFalling = false;
            isOnGround = true;
        }

        //===================================================

        //====================== Draw =============================

        BeginDrawing();

        ClearBackground(RAYWHITE);

        // Draw the map
        DrawTiled(map, 0, 0, WHITE);

        DrawRectangleV(player.position, player.size, MAROON);

        DrawFPS(10, 10);

        EndDrawing();
        //===================================================
    }

    // De-Initialization
    //---------------------------------------------------------
    UnloadMap(map);
    CloseWindow(); // Close window and OpenGL context
    //===================================================
    return 0;
}

// TODO: create a header file and move these utility functions elsewhere
// TODO; implement bush sprites 
// TODO: implement collision with bush
// TODO: scoring system