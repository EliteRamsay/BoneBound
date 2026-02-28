#include "GameState.h"
#include "../World/WorldMap.h"
#include "../World/LocalMap.h"
#include "../Entities/Player.h"
#include "../Entities/NPC.h"
#include "../Entities/SpawnPoint.h"
#include "../UI/Menus.h"
#include "../UI/HUD.h"
#include "../Systems/Input.h"
#include "../Systems/SaveLoad.h"
#include "Camera.h"

void GameStartup()
{
    // CRITICAL: Initialize player with correct state
    player.worldPos.x = 2;
    player.worldPos.y = 2;
    player.localPos.x = LOCAL_MAP_WIDTH / 2;  // Safe default
    player.localPos.y = LOCAL_MAP_HEIGHT / 2; // Safe default
    player.isInLocalMap = false;  // MUST be false at start
    
    printf("GameStartup: player.isInLocalMap=%d\n", player.isInLocalMap);
}

void GameUpdate()
{
    if (shouldQuit) return;
    
    // Validate worldMap
    if (currentState == STATE_PLAYING && !worldMap) {
        currentState = STATE_TITLE;
        selectedOption = 0;
        return;
    }
    
    // Validate player position
    if (!player.isInLocalMap && worldMap) {
        if (player.worldPos.x < 0 || player.worldPos.x >= currentMapWidth ||
            player.worldPos.y < 0 || player.worldPos.y >= currentMapHeight) {
            printf("Resetting invalid world position (%d,%d) to (2,2)\n", 
                   player.worldPos.x, player.worldPos.y);
            player.worldPos.x = 2;
            player.worldPos.y = 2;
        }
    }
    
    // Validate local position (but don't use it if not in local map)
    if (player.localPos.x < 0 || player.localPos.x >= LOCAL_MAP_WIDTH ||
        player.localPos.y < 0 || player.localPos.y >= LOCAL_MAP_HEIGHT) {
        player.localPos.x = LOCAL_MAP_WIDTH / 2;
        player.localPos.y = LOCAL_MAP_HEIGHT / 2;
    }
    
    // Handle fullscreen
    if (IsKeyPressed(KEY_F)) {
        ToggleFullscreen(SCREEN_WIDTH, SCREEN_HEIGHT);
    }
    
    switch (currentState) {
        case STATE_TITLE:
            TitleUpdate();
            break;
            
        case STATE_MAPSIZE:
            MapSizeUpdate();
            break;
            
        case STATE_SAVE_MENU:
            SaveMenuUpdate();
            break;
            
        case STATE_LOAD_MENU:
            LoadMenuUpdate();
            break;
            
        case STATE_PLAYING:
            HandleInput();
            UpdateCamera();
            UpdateNPCs();
            UpdateSpawnPoints();
            
            if (IsKeyPressed(KEY_F5)) {
                currentState = STATE_SAVE_MENU;
                saveSlotSelected = 0;
            }
            if (IsKeyPressed(KEY_F9)) {
                currentState = STATE_LOAD_MENU;
                saveSlotSelected = 0;
            }
            break;
    }
}

void GameDraw()
{
    BeginDrawing();
    ClearBackground(BLACK);
    
    switch (currentState) {
        case STATE_TITLE:     TitleDraw(); break;
        case STATE_MAPSIZE:   MapSizeDraw(); break;
        case STATE_SAVE_MENU: SaveMenuDraw(); break;
        case STATE_LOAD_MENU: LoadMenuDraw(); break;
        case STATE_PLAYING:
            BeginMode2D(gameCamera.camera);
            
            if (player.isInLocalMap) {
                DrawLocalMap();
                DrawNPCs();
                DrawLocalPlayer();
            } else {
                DrawWorldMap();
                DrawWorldPlayer();
            }
            
            EndMode2D();
            DrawHUD();
            break;
    }
    
    EndDrawing();
}