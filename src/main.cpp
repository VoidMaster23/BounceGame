#include "raylib.h"
#include <iostream>

#define RAYLIB_TILESON_IMPLEMENTATION
#include "raylib-tileson.hpp"

struct InitParams
{
    Vector2 ballPosition{};
    Vector2 ballSpeed{};
    int ballRadius{};
    bool isPaused{};
    int framesCounter{};
};

/*
Initializes the window as well  as basic start parameters for the game
*/
InitParams init(float groundY)
{
    const int screenWidth{1920};
    const int screenHeight{1080};

    SetConfigFlags(FLAG_MSAA_4X_HINT); // Anti Aliasing to Smooth Edges
    InitWindow(screenWidth, screenHeight, "Bounce");

    int ballRadius{20};
    Vector2 ballPosition{GetScreenWidth() / 2.0f, groundY};
    Vector2 ballSpeed{5.0f, 4.0f};

    bool isPaused{false};
    int framesCounter{0};

    SetTargetFPS(60); // Sets the game to 60fps

    return {
        .framesCounter = framesCounter,
        .isPaused = isPaused,
        .ballPosition = ballPosition, // This is a reference to the vec2D
        .ballSpeed = ballSpeed,
        .ballRadius = ballRadius};
}

int main(void)
{

    const int screenWidth{1920};
    const int screenHeight{1080};

    SetConfigFlags(FLAG_MSAA_4X_HINT); // Anti Aliasing to Smooth Edges
    InitWindow(screenWidth, screenHeight, "Bounce");

    bool isPaused{false};
    int framesCounter{0};

    SetTargetFPS(60); // Sets the game to 60fps

    Map map = LoadTiled(ASSETS_PATH "maps/BounceMap.json");

    if (!IsTiledReady(map))
    {
        return 1;
    }

    // Define the bounding box coordinates
    int minX = std::numeric_limits<int>::max();
    int minY = std::numeric_limits<int>::max();
    int maxX = std::numeric_limits<int>::min();
    int maxY = std::numeric_limits<int>::min();

    // Adjust this layer name or condition as per your map design
    std::string groundLayerName = "Ground";

    // Iterate through the map layers to find the ground layer
    for (auto &layer : map.data->map->getLayers())
    {
        if (layer.getType() == tson::LayerType::TileLayer && layer.getName() == groundLayerName)
        {
            // Iterate over the tiles in the tile layer
            for (const auto &[pos, tileObject] : layer.getTileObjects())
            {
                if (const_cast<tson::TileObject &>(tileObject).getTile())
                { // Check if there's an actual tile
                    // Extract the tuple into x and y
                    int tilePosX = std::get<0>(pos);
                    int tilePosY = std::get<1>(pos);

                    // Calculate tile's world position
                    int worldX = tilePosX * map.data->map->getTileSize().x;
                    int worldY = tilePosY * map.data->map->getTileSize().y;

                    // Update the bounding box coordinates
                    minX = std::min(minX, worldX);
                    minY = std::min(minY, worldY);
                    maxX = std::max(maxX, worldX + map.data->map->getTileSize().x);
                    maxY = std::max(maxY, worldY + map.data->map->getTileSize().y);
                }
            }
        }
    }

    // Output the bounding box
    if (minX <= maxX && minY <= maxY)
    {
        std::cout << "Ground Bounding Box:" << std::endl;
        std::cout << "Min X: " << minX << ", Min Y: " << minY << std::endl;
        std::cout << "Max X: " << maxX << ", Max Y: " << maxY << std::endl;
        std::cout << "Width: " << (maxX - minX) << ", Height: " << (maxY - minY) << std::endl;
    }
    else
    {
        std::cout << "No ground tiles found." << std::endl;
    }

    // Initialization
    //---------------------------------------------------------
    int ballRadius{20};
    Vector2 ballPosition{GetScreenWidth() / 2.0f, static_cast<float>(minY)};
    Vector2 ballSpeed{5.0f, 4.0f};
    // InitParams initValues{init(static_cast<float>(minY))};
    //----------------------------------------------------------
    bool isRising{false};
    bool isFalling{false};
    bool isOnGround{(ballPosition.y == static_cast<float>(minY))};

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {

        // input
        //-----------------------------------------------------
        if ((IsKeyDown(KEY_UP) || IsKeyDown(KEY_SPACE)) && isOnGround)
        {
            isRising = true;
            isOnGround = false;
        }

        // Update
        // -------------------------------------------------------
        float deltaTime = GetFrameTime();

        // Checks to see if we haven't reached the max jump height
        // TODO: refactor the check for max height
        if (isRising && (ballPosition.y > (GetScreenHeight() - 4 * ballRadius - 20.0f * ballSpeed.y)))
        {
            ballPosition.y -= (0.5 * ballSpeed.y * (pow(100 * deltaTime, 2)));
        }

        if (isRising && (ballPosition.y <= (GetScreenHeight() - 4 * ballRadius - 20.0f * ballSpeed.y)))
        {
            isRising = false;
            isFalling = true;
        }

        if (isFalling && (ballPosition.y < static_cast<float>(minY)))
        {
            ballPosition.y += (0.5 * ballSpeed.y * (pow(100 * deltaTime, 2)));
        }

        if (isFalling && (ballPosition.y >= static_cast<float>(minY)))
        {
            isFalling = false;
            isOnGround = true;
        }

        //-----------------------------------------------------

        // Draw
        //-----------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        // Draw the map
        DrawTiled(map, 0, 0, WHITE);

        DrawCircleV(ballPosition, (float)ballRadius, MAROON);

        DrawFPS(10, 10);

        EndDrawing();
        //-----------------------------------------------------
    }

    // De-Initialization
    //---------------------------------------------------------
    UnloadMap(map);
    CloseWindow(); // Close window and OpenGL context
    //------------------
    return 0;
}
