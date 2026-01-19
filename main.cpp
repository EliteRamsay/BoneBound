#include "project.h"

int main()
{
    // Start with default screen size
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "BoneBound");
    
    SetTargetFPS(60);
    srand(time(NULL));
    
    gamestartup();
    
    // Don't generate initial map - let user choose from menu
    // The camera will be initialized when they select a map size
    
    while(!WindowShouldClose())
    {
        // Update Game
        gameupdate();
        // Draw Game
        gamedraw();
    }

    gameshutdown();
    CloseWindow();

    return 0;
}