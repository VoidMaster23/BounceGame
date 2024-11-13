#include "raylib.h"
#include "raymath.h"

int main(void)
{
    // Initialize window and OpenGL context
    InitWindow(400, 224, "Template-Window");
    while (!WindowShouldClose())   // checks if a window close event has happened - user closing app
    {
        BeginDrawing();            // sets up the frame buffer
        ClearBackground(RAYWHITE); // sets bg color to white

        EndDrawing(); // ends buffer
    }

    CloseWindow();
    return 0;
}