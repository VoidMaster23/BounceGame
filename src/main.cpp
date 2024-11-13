#include "raylib.h"
#include <iostream>

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
InitParams init()
{
    const int screenWidth{800};
    const int screenHeight{450};

    SetConfigFlags(FLAG_MSAA_4X_HINT); // Anti Aliasing to Smooth Edges
    InitWindow(screenWidth, screenHeight, "Bounce");

    int ballRadius{20};
    Vector2 ballPosition{GetScreenWidth() / 2.0f, static_cast<float>(GetScreenHeight()) - 4 * ballRadius};
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

    // Initialization
    //---------------------------------------------------------
    InitParams initValues{init()};
    //----------------------------------------------------------
    bool isRising{false};
    bool isFalling{false};
    bool isOnGround{(initValues.ballPosition.y == (GetScreenHeight() - 4 * initValues.ballRadius))};

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {

        // input
        //-----------------------------------------------------
        if ((IsKeyDown(KEY_UP) || IsKeyDown(KEY_SPACE)) && isOnGround)
        {
            isRising = true;
            isOnGround = false;
            std::cout << initValues.ballSpeed.y << std::endl;
        }

        // Update
        // -------------------------------------------------------
        float deltaTime = GetFrameTime();

        if (isRising && (initValues.ballPosition.y > (GetScreenHeight() - 4 * initValues.ballRadius - 20.0f * initValues.ballSpeed.y)))
        {
            std::cout << "RISING" << std::endl;
            initValues.ballPosition.y -= (0.5 * initValues.ballSpeed.y * (pow(100 * deltaTime, 2)));
        }

        if (isRising && (initValues.ballPosition.y <= (GetScreenHeight() - 4 * initValues.ballRadius - 20.0f * initValues.ballSpeed.y)))
        {
            isRising = false;
            isFalling = true;
            std::cout << "SETTING FALL" << std::endl;
        }

        if (isFalling && (initValues.ballPosition.y < (GetScreenHeight() - 4 * initValues.ballRadius)))
        {
            std::cout << "FALLING" << std::endl;
            initValues.ballPosition.y += (0.5 * initValues.ballSpeed.y * (pow(100 * deltaTime, 2)));
        }

        if (isFalling && (initValues.ballPosition.y >= (GetScreenHeight() - 4 * initValues.ballRadius)))
        {
            std::cout << "BACK TO GROUND" << std::endl;
            isFalling = false;
            isOnGround = true;
        }

        //-----------------------------------------------------

        // Draw
        //-----------------------------------------------------
        BeginDrawing();

        ClearBackground(GetColor(0x052c46ff));

        DrawCircleV(initValues.ballPosition, (float)initValues.ballRadius, MAROON);

        DrawFPS(10, 10);

        EndDrawing();
        //-----------------------------------------------------
    }

    // De-Initialization
    //---------------------------------------------------------
    CloseWindow();             // Close window and OpenGL context
    //------------------
    return 0;
}
