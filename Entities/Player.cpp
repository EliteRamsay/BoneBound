#include "Player.h"
#include "NPC.h"
#include "../World/LocalMap.h"
#include "../Game/Camera.h"
#include "../Systems/PositionValidation.h"
#include <stdio.h>

void UpdatePlayer()
{
    if (currentState != STATE_PLAYING) return;
    
    // CRITICAL: Validate worldMap exists
    if (!worldMap) {
        printf("ERROR: worldMap is NULL in UpdatePlayer\n");
        return;
    }
    
    // Validate world position
    if (player.worldPos.x < 0 || player.worldPos.x >= currentMapWidth ||
        player.worldPos.y < 0 || player.worldPos.y >= currentMapHeight) {
        
        printf("ERROR: Invalid world position (%d,%d)! Resetting to (2,2)\n", 
               player.worldPos.x, player.worldPos.y);
        player.worldPos.x = 2;
        player.worldPos.y = 2;
        
        // Save to crash log
        FILE* log = fopen("crashlog.txt", "a");
        if (log) {
            fprintf(log, "--- Player position reset from invalid coordinates ---\n");
            fclose(log);
        }
    }
    
    // Ensure local position is always valid (even if not in local map)
    if (player.localPos.x < 0 || player.localPos.x >= LOCAL_MAP_WIDTH ||
        player.localPos.y < 0 || player.localPos.y >= LOCAL_MAP_HEIGHT) {
        player.localPos.x = LOCAL_MAP_WIDTH / 2;
        player.localPos.y = LOCAL_MAP_HEIGHT / 2;
    }
    
    Position* pos = player.isInLocalMap ? &player.localPos : &player.worldPos;
    int oldX = pos->x;
    int oldY = pos->y;
    
    if (player.isInLocalMap) {
        // ============= LOCAL MAP MOVEMENT =============
        
        // Get current local map
        WorldTile* tile = &worldMap[player.worldPos.y][player.worldPos.x];
        if (!tile->localMap) {
            printf("ERROR: localMap is NULL in UpdatePlayer - exiting local map\n");
            ExitLocalMap();
            return;
        }
        
        LocalMap* map = tile->localMap;
        
        int newX = pos->x;
        int newY = pos->y;
        
        // Right/D
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
            if (pos->x + 1 < map->width) {
                char destTile = map->tiles[pos->y][pos->x + 1];
                if (IsTileWalkable(destTile)) {
                    newX = pos->x + 1;
                }
            }
        }
        
        // Left/A
        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
            if (pos->x > 0) {
                char destTile = map->tiles[pos->y][pos->x - 1];
                if (IsTileWalkable(destTile)) {
                    newX = pos->x - 1;
                }
            }
        }
        
        // Up/W
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            if (pos->y > 0) {
                char destTile = map->tiles[pos->y - 1][pos->x];
                if (IsTileWalkable(destTile)) {
                    newY = pos->y - 1;
                }
            }
        }
        
        // Down/S
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
            if (pos->y + 1 < map->height) {
                char destTile = map->tiles[pos->y + 1][pos->x];
                if (IsTileWalkable(destTile)) {
                    newY = pos->y + 1;
                }
            }
        }
        
        pos->x = newX;
        pos->y = newY;
        
        // Exit local map with BACKSPACE
        if (IsKeyPressed(KEY_BACKSPACE)) {
            ExitLocalMap();
            return;
        }
        
        // Interact with NPCs
        if (IsKeyPressed(KEY_E) || IsKeyPressed(KEY_SPACE)) {
            InteractWithNPC();
        }
        // ==============================================
        
    } else {
        // ============= WORLD MAP MOVEMENT =============
        int newX = pos->x;
        int newY = pos->y;
        
        // Validate current position before moving
        if (pos->y < 0 || pos->y >= currentMapHeight || 
            pos->x < 0 || pos->x >= currentMapWidth) {
            printf("ERROR: Invalid position before movement\n");
            return;
        }
        
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
            if (pos->x + 1 < currentMapWidth) {
                char destTile = worldMap[pos->y][pos->x + 1].worldTile;
                if (destTile != '#') {
                    newX = pos->x + 1;
                }
            }
        }
        
        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
            if (pos->x > 0) {
                char destTile = worldMap[pos->y][pos->x - 1].worldTile;
                if (destTile != '#') {
                    newX = pos->x - 1;
                }
            }
        }
        
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            if (pos->y > 0) {
                char destTile = worldMap[pos->y - 1][pos->x].worldTile;
                if (destTile != '#') {
                    newY = pos->y - 1;
                }
            }
        }
        
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
            if (pos->y + 1 < currentMapHeight) {
                char destTile = worldMap[pos->y + 1][pos->x].worldTile;
                if (destTile != '#') {
                    newY = pos->y + 1;
                }
            }
        }
        
        pos->x = newX;
        pos->y = newY;
        
        if (IsKeyPressed(KEY_ENTER)) {
            if (pos->y >= 0 && pos->y < currentMapHeight && 
                pos->x >= 0 && pos->x < currentMapWidth) {
                WorldTile* tile = &worldMap[pos->y][pos->x];
                if (tile->hasLocalMap) {
                    EnterLocalMap(pos->x, pos->y);
                    return; // Important: return after entering local map
                }
            }
        }
        
        if (IsKeyPressed(KEY_BACKSPACE)) {
            currentState = STATE_TITLE;
            selectedOption = 0;
        }
        // ==============================================
    }
    
    if (oldX != pos->x || oldY != pos->y) {
        UpdateCamera();
    }
}

// ===== PLAYER DRAWING FUNCTIONS =====

void DrawWorldPlayer()
{
    // Safety check - should only be called when not in local map
    if (player.isInLocalMap) {
        printf("WARNING: DrawWorldPlayer called while in local map!\n");
        return;
    }
    
    // Validate world position
    if (player.worldPos.x < 0 || player.worldPos.x >= currentMapWidth ||
        player.worldPos.y < 0 || player.worldPos.y >= currentMapHeight) {
        printf("ERROR: Invalid world position (%d,%d) in DrawWorldPlayer\n",
               player.worldPos.x, player.worldPos.y);
        return;
    }
    
    Vector2 pos = {
        player.worldPos.x * TILE_SIZE + 8.0f,
        player.worldPos.y * TILE_SIZE + 6.0f
    };
    DrawTextEx(GetFontDefault(), "@", pos, 24, 1, YELLOW);
}

void DrawLocalPlayer()
{
    // Safety check - should only be called when in local map
    if (!player.isInLocalMap) {
        printf("WARNING: DrawLocalPlayer called while not in local map!\n");
        return;
    }
    
    // Validate local position
    if (player.localPos.x < 0 || player.localPos.x >= LOCAL_MAP_WIDTH ||
        player.localPos.y < 0 || player.localPos.y >= LOCAL_MAP_HEIGHT) {
        printf("CRITICAL: Invalid local position (%d,%d) in DrawLocalPlayer\n",
               player.localPos.x, player.localPos.y);
        return;
    }
    
    Vector2 pos = {
        player.localPos.x * TILE_SIZE + 8.0f,
        player.localPos.y * TILE_SIZE + 6.0f
    };
    DrawTextEx(GetFontDefault(), "@", pos, 24, 1, YELLOW);
}