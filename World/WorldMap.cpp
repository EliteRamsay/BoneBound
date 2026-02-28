#include "WorldMap.h"
#include "LocalMap.h"
#include "../Entities/NPC.h"
#include "../Entities/SpawnPoint.h"
#include "../Game/Camera.h"
#include <time.h>

// Add this helper function at the top of WorldMap.cpp
Color SafeGetTileColor(int x, int y)
{
    // Bounds checking
    if (x < 0 || x >= currentMapWidth || y < 0 || y >= currentMapHeight) {
        printf("WARNING: SafeGetTileColor called with invalid coordinates (%d,%d)\n", x, y);
        return RAYWHITE; // Return a safe default color
    }
    return GetTileColor(worldMap[y][x].worldTile);
}
const char* VILLAGE_NAMES[] = {
    "Oakhaven", "Riverwood", "Stonebridge", "Millbrook", "Fairvale",
    "Greenfield", "Wolfden", "Eastwick", "Westburton", "Northgate",
    "Southmere", "Dustmill", "Fernwood", "Maplecroft", "Willowcreek",
    "Ashford", "Briarwood", "Cresthill", "Dalebrook", "Elmshire"
};

const char* CASTLE_NAMES[] = {
    "Dragonspire", "Stormhold", "Ironkeep", "Winterfell", "Highgarden",
    "Blackrock", "Shadowfort", "Ravencrest", "Eagle's Nest", "Wolf's Crag",
    "Dreadfort", "Casterly Rock", "Riverrun", "Pyke", "Sunspear"
};

const char* CAVE_NAMES[] = {
    "Gloomdeep", "Echochasm", "Darkmaw", "Crystal Cavern", "Shadowmere",
    "Whispering Grotto", "Stalactite Cave", "Fungal Depths", "Goblin Warrens",
    "Troll's Den", "Bat Cave", "Lost Passage", "Deepholm", "Obsidian Cave"
};

const char* TOWER_NAMES[] = {
    "Starweaver's Spire", "Archmage's Tower", "Celestial Observatory",
    "Wizard's End", "Sorcerer's Stone", "Enchanter's Rise", "Mystic Spire",
    "Arcanum Tower", "Spellweaver's Keep", "Magus Hold", "Crystal Spire"
};

const char* DUNGEON_NAMES[] = {
    "Torture Pits", "Soulprison", "Agony's End", "Barrow Deep",
    "Crypt of Sorrow", "Despair Depths", "Eternal Labyrinth",
    "Forsaken Dungeon", "Griefhold", "Horror's Gate", "Infernal Keep"
};

const char* RUINS_NAMES[] = {
    "Fallen Kingdom", "Lost City", "Ancient Remains", "Crumbled Spire",
    "Deserted Village", "Echoing Ruins", "Forgotten Citadel",
    "Ghostly Remains", "Haunted Grounds", "Ivory Debris"
};

const char* FORT_NAMES[] = {
    "Stoneguard", "Ironwall", "Steelkeep", "Battleford", "Warrenhold",
    "Defender's Rest", "Garrison Point", "Hammerfall", "Outpost Omega",
    "Shieldwall", "Vanguard", "Watchtower", "Bulwark"
};

const char* GetRandomLocationName(LocationType type)
{
    int index;
    switch (type) {
        case LOCATION_VILLAGE:
            index = rand() % (sizeof(VILLAGE_NAMES) / sizeof(VILLAGE_NAMES[0]));
            return VILLAGE_NAMES[index];
        case LOCATION_CASTLE:
            index = rand() % (sizeof(CASTLE_NAMES) / sizeof(CASTLE_NAMES[0]));
            return CASTLE_NAMES[index];
        case LOCATION_CAVE:
            index = rand() % (sizeof(CAVE_NAMES) / sizeof(CAVE_NAMES[0]));
            return CAVE_NAMES[index];
        case LOCATION_WIZARD_TOWER:
            index = rand() % (sizeof(TOWER_NAMES) / sizeof(TOWER_NAMES[0]));
            return TOWER_NAMES[index];
        case LOCATION_DUNGEON:
            index = rand() % (sizeof(DUNGEON_NAMES) / sizeof(DUNGEON_NAMES[0]));
            return DUNGEON_NAMES[index];
        case LOCATION_RUINS:
            index = rand() % (sizeof(RUINS_NAMES) / sizeof(RUINS_NAMES[0]));
            return RUINS_NAMES[index];
        case LOCATION_FORT:
            index = rand() % (sizeof(FORT_NAMES) / sizeof(FORT_NAMES[0]));
            return FORT_NAMES[index];
        default:
            return "Unknown";
    }
}

char GetLocationChar(LocationType type)
{
    switch (type) {
        case LOCATION_VILLAGE: return 'V';
        case LOCATION_CASTLE: return 'C';
        case LOCATION_CAVE: return 'c';
        case LOCATION_WIZARD_TOWER: return 'W';
        case LOCATION_DUNGEON: return 'D';
        case LOCATION_RUINS: return 'R';
        case LOCATION_FORT: return 'F';
        case LOCATION_TEMPLE: return 'T';
        case LOCATION_NONE: return '?';  // Add this
        default: 
            printf("WARNING: Unknown location type %d\n", type);
            return '?';
    }
}

Color GetLocationColor(LocationType type, Color baseColor)
{
    switch (type) {
        case LOCATION_VILLAGE: return (Color){ 100, 255, 100, 255 };
        case LOCATION_CASTLE: return (Color){ 200, 200, 255, 255 };
        case LOCATION_CAVE: return (Color){ 139, 69, 19, 255 };
        case LOCATION_WIZARD_TOWER: return (Color){ 200, 100, 255, 255 };
        case LOCATION_DUNGEON: return (Color){ 150, 0, 0, 255 };
        case LOCATION_RUINS: return (Color){ 169, 169, 169, 255 };
        case LOCATION_FORT: return (Color){ 128, 128, 128, 255 };
        case LOCATION_TEMPLE: return (Color){ 255, 215, 0, 255 };
        case LOCATION_NONE: return baseColor;  // Add this
        default: 
            printf("WARNING: Unknown location type %d in GetLocationColor\n", type);
            return baseColor;
    }
}

LocationType DetermineLocationType(char terrain, int x, int y, int width, int height)
{
    if (x == 0 || x == width - 1 || y == 0 || y == height - 1)
        return LOCATION_NONE;
    
    int randVal = rand() % 100;
    
    switch (terrain) {
        case '.': // Grasslands
            if (randVal < 30) return LOCATION_VILLAGE;
            else if (randVal < 40) return LOCATION_FORT;
            else if (randVal < 45) return LOCATION_RUINS;
            else if (randVal < 48) return LOCATION_CASTLE;
            else return LOCATION_NONE;
            
        case 'T': // Forest
            if (randVal < 20) return LOCATION_VILLAGE;
            else if (randVal < 30) return LOCATION_RUINS;
            else if (randVal < 35) return LOCATION_CAVE;
            else if (randVal < 38) return LOCATION_WIZARD_TOWER;
            else return LOCATION_NONE;
            
        case '^': // Mountains
            if (randVal < 25) return LOCATION_CAVE;
            else if (randVal < 35) return LOCATION_DUNGEON;
            else if (randVal < 40) return LOCATION_RUINS;
            else if (randVal < 43) return LOCATION_CASTLE;
            else if (randVal < 45) return LOCATION_FORT;
            else return LOCATION_NONE;
            
        case '~': // Water
            if (randVal < 5) return LOCATION_RUINS;
            else if (randVal < 7) return LOCATION_CAVE;
            else return LOCATION_NONE;
            
        default:
            return LOCATION_NONE;
    }
}

void DrawWorldMap()
{
    if (worldMap == NULL) {
        printf("Error: worldMap is NULL in DrawWorldMap\n");
        return;
    }
    
    // Validate player position
    if (player.worldPos.y < 0 || player.worldPos.y >= currentMapHeight ||
        player.worldPos.x < 0 || player.worldPos.x >= currentMapWidth) {
        printf("CRASH PREVENTION: Invalid world position (%d,%d) in DrawWorldMap\n", 
               player.worldPos.x, player.worldPos.y);
        player.worldPos.x = 2;
        player.worldPos.y = 2;
        return;
    }
    
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    float visibleWidth = screenWidth / gameCamera.camera.zoom;
    float visibleHeight = screenHeight / gameCamera.camera.zoom;
    
    Vector2 cameraWorldTopLeft = {
        gameCamera.camera.target.x - visibleWidth / 2.0f,
        gameCamera.camera.target.y - visibleHeight / 2.0f
    };
    
    DrawRectangle(cameraWorldTopLeft.x, cameraWorldTopLeft.y, 
                  visibleWidth, visibleHeight, BLACK);
    
    int startX = (int)(cameraWorldTopLeft.x / TILE_SIZE) - 1;
    int startY = (int)(cameraWorldTopLeft.y / TILE_SIZE) - 1;
    int endX = (int)((cameraWorldTopLeft.x + visibleWidth) / TILE_SIZE) + 2;
    int endY = (int)((cameraWorldTopLeft.y + visibleHeight) / TILE_SIZE) + 2;
    
    startX = (startX < 0) ? 0 : startX;
    startY = (startY < 0) ? 0 : startY;
    endX = (endX > currentMapWidth) ? currentMapWidth : endX;
    endY = (endY > currentMapHeight) ? currentMapHeight : endY;
    
    for(int y = startY; y < endY; y++)
    {
        for(int x = startX; x < endX; x++)
        {
            // CRITICAL: Check array bounds
            if (y < 0 || y >= currentMapHeight || x < 0 || x >= currentMapWidth) {
                continue;
            }
            
            WorldTile* tile = &worldMap[y][x];
            
            // SAFETY: Check if tile pointer is valid
            if (tile == NULL) {
                printf("ERROR: NULL tile at (%d,%d)\n", x, y);
                continue;
            }
            
            char displayChar;
            Color color = GetTileColor(tile->worldTile);
            
            // Check if this is a location - but ONLY if tile is valid
            if (tile->locationType != LOCATION_NONE) {
                displayChar = GetLocationChar(tile->locationType);
                color = GetLocationColor(tile->locationType, color);
            } else {
                displayChar = tile->worldTile;
            }
            
            // SAFETY: Check discovered flag exists (it should, but just in case)
            if (!tile->discovered) {
                color.r = color.r / 2;
                color.g = color.g / 2;
                color.b = color.b / 2;
            }
            
            Vector2 pos = { 
                (float)(x * TILE_SIZE + 8), 
                (float)(y * TILE_SIZE + 6) 
            };
            
            int fontSize = (currentMapWidth <= 8 && currentMapHeight <= 8) ? 28 : 24;
            
            DrawTextEx(
                GetFontDefault(),
                TextFormat("%c", displayChar),
                pos,
                fontSize,
                1,
                color);
        }
    }
}

void GenerateWorldMap(int width, int height)
{
    printf("Generating world map %dx%d\n", width, height);
    
    CleanupAllMaps();
    
    currentMapWidth = width;
    currentMapHeight = height;
    
    worldMap = (WorldTile**)malloc(height * sizeof(WorldTile*));
    if (!worldMap) {
        printf("ERROR: Failed to allocate worldMap rows\n");
        return;
    }
    
    for (int y = 0; y < height; y++) {
        worldMap[y] = (WorldTile*)malloc(width * sizeof(WorldTile));
        if (!worldMap[y]) {
            printf("ERROR: Failed to allocate worldMap row %d\n", y);
            return;
        }
        
        for (int x = 0; x < width; x++) {
            // Initialize ALL fields
            worldMap[y][x].worldTile = '#';
            worldMap[y][x].localMap = NULL;
            worldMap[y][x].hasLocalMap = false;
            worldMap[y][x].locationType = LOCATION_NONE;
            worldMap[y][x].locationName[0] = '\0';
            worldMap[y][x].discovered = false;
            worldMap[y][x].lastExitX = LOCAL_MAP_WIDTH / 2;
            worldMap[y][x].lastExitY = LOCAL_MAP_HEIGHT / 2;
            worldMap[y][x].hasExitPoint = false;
            
            if (!(x == 0 || x == width - 1 || y == 0 || y == height - 1)) {
                float randVal = (float)rand() / RAND_MAX;
                
                if (randVal < 0.40) {
                    worldMap[y][x].worldTile = '.';
                } else if (randVal < 0.65) {
                    worldMap[y][x].worldTile = 'T';
                } else if (randVal < 0.85) {
                    worldMap[y][x].worldTile = '^';
                } else {
                    worldMap[y][x].worldTile = '~';
                }
                
                worldMap[y][x].hasLocalMap = true;
                worldMap[y][x].locationType = DetermineLocationType(
                    worldMap[y][x].worldTile, x, y, width, height);
                
                if (worldMap[y][x].locationType != LOCATION_NONE) {
                    strcpy(worldMap[y][x].locationName, 
                           GetRandomLocationName(worldMap[y][x].locationType));
                } else {
                    switch (worldMap[y][x].worldTile) {
                        case '.': strcpy(worldMap[y][x].locationName, "Grasslands"); break;
                        case 'T': strcpy(worldMap[y][x].locationName, "Forest"); break;
                        case '^': strcpy(worldMap[y][x].locationName, "Mountains"); break;
                        case '~': strcpy(worldMap[y][x].locationName, "Lake"); break;
                    }
                }
            }
        }
    }
    
    // Set starting village
    if (height > 2 && width > 2) {
        worldMap[2][2].worldTile = '.';
        worldMap[2][2].locationType = LOCATION_VILLAGE;
        worldMap[2][2].hasLocalMap = true;
        strcpy(worldMap[2][2].locationName, "Starting Village");
        worldMap[2][2].discovered = true;
        worldMap[2][2].hasExitPoint = false;
    }
    
    // Clear area around start
    for (int y = 1; y <= 3 && y < height; y++) {
        for (int x = 1; x <= 3 && x < width; x++) {
            worldMap[y][x].worldTile = '.';
        }
    }
    
    // CRITICAL: Set player state BEFORE camera init
    player.worldPos.x = 2;
    player.worldPos.y = 2;
    player.isInLocalMap = false;  // Make sure this is false
    
    printf("World map generated successfully\n");
    printf("Starting tile at (2,2): type=%c, hasLocalMap=%d, locationType=%d\n",
           worldMap[2][2].worldTile,
           worldMap[2][2].hasLocalMap,
           worldMap[2][2].locationType);
    
    // Use SafeInitCamera
    SafeInitCamera(true);
}

void ValidateWorldMap()
{
    if (!worldMap) {
        printf("ERROR: worldMap is NULL in validation\n");
        return;
    }
    
    printf("Validating world map %dx%d...\n", currentMapWidth, currentMapHeight);
    int errors = 0;
    
    for (int y = 0; y < currentMapHeight; y++) {
        if (!worldMap[y]) {
            printf("ERROR: worldMap[%d] is NULL\n", y);
            errors++;
            continue;
        }
        
        for (int x = 0; x < currentMapWidth; x++) {
            WorldTile* tile = &worldMap[y][x];
            
            // Basic validation
            if (tile->worldTile == '\0') {
                printf("ERROR: Tile (%d,%d) has null worldTile\n", x, y);
                errors++;
            }
            
            // Check location name if it's a location
            if (tile->locationType != LOCATION_NONE && tile->locationName[0] == '\0') {
                printf("ERROR: Location tile (%d,%d) has no name\n", x, y);
                errors++;
            }
        }
    }
    
    if (errors == 0) {
        printf("World map validation passed!\n");
    } else {
        printf("World map validation found %d errors\n", errors);
    }
}

void CleanupAllMaps()
{
    if (!worldMap) return;
    
    for (int y = 0; y < currentMapHeight; y++) {
        for (int x = 0; x < currentMapWidth; x++) {
            if (worldMap[y][x].localMap) {
                LocalMap* local = worldMap[y][x].localMap;
                
                if (local->spawnPoints) {
                    for (int i = 0; i < local->spawnPointCount; i++)
                        free(local->spawnPoints[i]);
                    free(local->spawnPoints);
                }
                
                if (local->buildings) {
                    for (int i = 0; i < local->buildingCount; i++) {
                        Building* b = local->buildings[i];
                        for (int j = 0; j < b->height; j++)
                            free(b->layout[j]);
                        free(b->layout);
                        free(b);
                    }
                    free(local->buildings);
                }
                
                for (int ly = 0; ly < local->height; ly++)
                    free(local->tiles[ly]);
                free(local->tiles);
                free(local);
            }
        }
        free(worldMap[y]);
    }
    free(worldMap);
    worldMap = NULL;
    
    for (int i = 0; i < npcCount; i++)
        free(npcs[i]);
    free(npcs);
    npcs = NULL;
    npcCount = 0;
}