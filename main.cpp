#include "project.h"

int main()
{
    // Initialize window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "BoneBound");
    SetTargetFPS(60);
    
    // Initialize game
    gamestartup();
    
    // Main game loop
    while (!WindowShouldClose())
    {
        gameupdate();
        gamedraw();
    }
    
    // Cleanup
    gameshutdown();
    CloseWindow();
    
    return 0;
}