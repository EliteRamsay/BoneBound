#include "SaveLoad.h"
#include "../World/WorldMap.h"
#include "../World/LocalMap.h"
#include "../Entities/NPC.h"
#include "../Game/Camera.h"

bool SaveFileExists(int slot)
{
    char filename[50];
    sprintf(filename, "save_%d.dat", slot);
    FILE* f = fopen(filename, "rb");
    if (f) {
        fclose(f);
        return true;
    }
    return false;
}

void SaveGame(int slot)
{
    char filename[50];
    sprintf(filename, "save_%d.dat", slot);
    FILE* file = fopen(filename, "wb");
    if (!file) return;
    
    // Save world dimensions
    fwrite(&currentMapWidth, sizeof(int), 1, file);
    fwrite(&currentMapHeight, sizeof(int), 1, file);
    
    // Save player
    fwrite(&player.worldPos, sizeof(Position), 1, file);
    fwrite(&player.localPos, sizeof(Position), 1, file);
    fwrite(&player.isInLocalMap, sizeof(bool), 1, file);
    
    // Save world map
    for (int y = 0; y < currentMapHeight; y++) {
        for (int x = 0; x < currentMapWidth; x++) {
            fwrite(&worldMap[y][x].worldTile, sizeof(char), 1, file);
            fwrite(&worldMap[y][x].hasLocalMap, sizeof(bool), 1, file);
            
            bool hasLocal = (worldMap[y][x].localMap != NULL);
            fwrite(&hasLocal, sizeof(bool), 1, file);
            
            if (hasLocal) {
                LocalMap* local = worldMap[y][x].localMap;
                fwrite(&local->width, sizeof(int), 1, file);
                fwrite(&local->height, sizeof(int), 1, file);
                
                // Save tiles
                for (int ly = 0; ly < local->height; ly++)
                    fwrite(local->tiles[ly], sizeof(char), local->width, file);
            }
        }
    }
    
    fclose(file);
}

bool LoadGame(int slot)
{
    char filename[50];
    sprintf(filename, "save_%d.dat", slot);
    FILE* file = fopen(filename, "rb");
    if (!file) return false;
    
    CleanupAllMaps();
    
    // Load world dimensions
    fread(&currentMapWidth, sizeof(int), 1, file);
    fread(&currentMapHeight, sizeof(int), 1, file);
    
    // Load player
    fread(&player.worldPos, sizeof(Position), 1, file);
    fread(&player.localPos, sizeof(Position), 1, file);
    fread(&player.isInLocalMap, sizeof(bool), 1, file);
    
    // Validate loaded player state
    if (player.worldPos.x < 0 || player.worldPos.x >= currentMapWidth ||
        player.worldPos.y < 0 || player.worldPos.y >= currentMapHeight) {
        player.worldPos.x = 2;
        player.worldPos.y = 2;
    }
    
    if (player.localPos.x < 0 || player.localPos.x >= LOCAL_MAP_WIDTH ||
        player.localPos.y < 0 || player.localPos.y >= LOCAL_MAP_HEIGHT) {
        player.localPos.x = LOCAL_MAP_WIDTH / 2;
        player.localPos.y = LOCAL_MAP_HEIGHT / 2;
    }
    
    // Allocate world map
    worldMap = (WorldTile**)malloc(currentMapHeight * sizeof(WorldTile*));
    for (int y = 0; y < currentMapHeight; y++) {
        worldMap[y] = (WorldTile*)malloc(currentMapWidth * sizeof(WorldTile));
        
        for (int x = 0; x < currentMapWidth; x++) {
            fread(&worldMap[y][x].worldTile, sizeof(char), 1, file);
            fread(&worldMap[y][x].hasLocalMap, sizeof(bool), 1, file);
            
            bool hasLocal;
            fread(&hasLocal, sizeof(bool), 1, file);
            
            if (hasLocal) {
                LocalMap* local = (LocalMap*)malloc(sizeof(LocalMap));
                fread(&local->width, sizeof(int), 1, file);
                fread(&local->height, sizeof(int), 1, file);
                
                local->tiles = (char**)malloc(local->height * sizeof(char*));
                for (int ly = 0; ly < local->height; ly++) {
                    local->tiles[ly] = (char*)malloc((local->width + 1) * sizeof(char));
                    fread(local->tiles[ly], sizeof(char), local->width, file);
                    local->tiles[ly][local->width] = '\0';
                }
                
                local->spawnPoints = NULL;
                local->spawnPointCount = 0;
                worldMap[y][x].localMap = local;
            } else {
                worldMap[y][x].localMap = NULL;
            }
        }
    }
    
    fclose(file);
    
    // FIX: Add the required parameter
    SafeInitCamera(true);  // or SafeInitCamera(false) depending on your needs
    
    return true;
}