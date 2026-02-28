#include "project.h"
#include "Game/GameState.h"
#include "World/WorldMap.h"
#include "Entities/Player.h"
#include "Systems/SaveLoad.h"
#include "Systems/CrashHandler.h"
#include "Systems/Logger.h"  // ADD THIS

int main()
{
    //InitLogger();  // ADD THIS
    LOG("Program started\n");
    
    SetupCrashHandler();
    
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "BoneBound");
    LOG("Window initialized\n");
    
    SetTargetFPS(60);
    InitAudioDevice();
    srand(time(NULL));
    
    memset(&player, 0, sizeof(PlayerData));
    player.worldPos.x = 2;
    player.worldPos.y = 2;
    player.localPos.x = LOCAL_MAP_WIDTH / 2;
    player.localPos.y = LOCAL_MAP_HEIGHT / 2;
    player.isInLocalMap = false;
    
    LOG("Player initialized at (%d,%d)\n", player.worldPos.x, player.worldPos.y);
    
    currentState = STATE_TITLE;
    shouldQuit = false;
    
    while (!WindowShouldClose() && !shouldQuit)
    {
        static int frameCounter = 0;
        if (++frameCounter % 1800 == 0) {
            CheckMemoryUsage();
        }
        
        GameUpdate();
        GameDraw();
    }
    
    CleanupAllMaps();
    CloseAudioDevice();
    CloseWindow();
    
    //CloseLogger();  // ADD THIS
    return 0;
}