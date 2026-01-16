#include "project.h"

int main()
{
    InitWindow(
        MAP_WIDTH*TILE_SIZE,
        MAP_HEIGHT*TILE_SIZE,
         "BoneBound");
    SetTargetFPS(60);
    srand(time(NULL));
    gamestartup();
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