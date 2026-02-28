#include "HUD.h"

void DrawHUD()
{
    int screenHeight = GetScreenHeight();
    int screenWidth = GetScreenWidth();
    
    if (player.isInLocalMap) {
        // SAFE: We're in a local map, so localPos is valid
        WorldTile* tile = &worldMap[player.worldPos.y][player.worldPos.x];
        
        Vector2 locSize = MeasureTextEx(GetFontDefault(), tile->locationName, 28, 1);
        Vector2 locPos = { screenWidth / 2.0f - locSize.x / 2.0f, 15 };
        DrawTextEx(GetFontDefault(), tile->locationName, locPos, 28, 1, GOLD);
        
        const char* typeStr;
        switch (tile->locationType) {
            case LOCATION_VILLAGE: typeStr = "Village"; break;
            case LOCATION_CASTLE: typeStr = "Castle"; break;
            case LOCATION_CAVE: typeStr = "Cave"; break;
            case LOCATION_WIZARD_TOWER: typeStr = "Wizard's Tower"; break;
            case LOCATION_DUNGEON: typeStr = "Dungeon"; break;
            case LOCATION_RUINS: typeStr = "Ruins"; break;
            case LOCATION_FORT: typeStr = "Fort"; break;
            case LOCATION_TEMPLE: typeStr = "Temple"; break;
            default: typeStr = "Area";
        }
        
        Vector2 typeSize = MeasureTextEx(GetFontDefault(), typeStr, 18, 1);
        Vector2 typePos = { screenWidth / 2.0f - typeSize.x / 2.0f, 50 };
        DrawTextEx(GetFontDefault(), typeStr, typePos, 18, 1, LIGHTGRAY);
        
        DrawText("BACKSPACE: Exit | F5: Save | F9: Load", 10, screenHeight - 30, 18, LIGHTGRAY);
        
        // SAFE: Only show local position when in local map
        DrawText(TextFormat("Local Pos: (%d,%d)", player.localPos.x, player.localPos.y),
                10, screenHeight - 55, 18, LIGHTGRAY);
        
        DrawText(TextFormat("NPCs: %d", npcCount), 10, screenHeight - 80, 18, LIGHTGRAY);
    } else {
        // We're on world map - NEVER use player.localPos here!
        WorldTile* tile = &worldMap[player.worldPos.y][player.worldPos.x];
        
        if (tile->hasLocalMap && tile->discovered && gameCamera.zoom > 1.5f) {
            DrawText(TextFormat("%s", tile->locationName), 10, screenHeight - 55, 18, GOLD);
        } else if (tile->hasLocalMap && !tile->discovered) {
            DrawText("Unknown Location", 10, screenHeight - 55, 18, GRAY);
        }
        
        DrawText("ENTER: Enter | F5: Save | F9: Load", 10, screenHeight - 30, 18, LIGHTGRAY);
        
        // Only show world position, NEVER local position
        DrawText(TextFormat("World: (%d,%d) | Terrain: %c", 
            player.worldPos.x, player.worldPos.y,
            tile->worldTile), 10, screenHeight - 80, 18, LIGHTGRAY);
    }
    
    // Common controls
    DrawText("WASD: Move | R: Reset Camera | Wheel: Zoom | F: Fullscreen | E: Talk", 
             10, screenHeight - 105, 16, DARKGRAY);
}