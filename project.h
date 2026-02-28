#ifndef PROJECT_H
#define PROJECT_H

// ===== RAYLIB INCLUDES =====
#include <raylib.h>
#include <raymath.h>

// ===== STANDARD LIBRARIES =====
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "Systems/Logger.h"
// ===== GAME CONSTANTS =====
#define TILE_SIZE 32
#define LOCAL_MAP_WIDTH 256
#define LOCAL_MAP_HEIGHT 256
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define DEFAULT_WORLD_WIDTH 20
#define DEFAULT_WORLD_HEIGHT 15
#define MAX_NPCS 1000
#define MAX_SPAWN_POINTS 40
#define MAX_SAVE_SLOTS 3
#define ROAD_WIDTH 2
#define MAX_NPCS_PER_MAP 100  // Hard limit per local map
#define MAX_TOTAL_NPCS 5000    // Global limit for all maps

// ===== ENUMS =====
typedef enum {
    SIZE_TINY, SIZE_SMALL, SIZE_MEDIUM,
    SIZE_LARGE, SIZE_HUGE, SIZE_GIGANTIC,
    NUM_SIZES
} MapSize;

typedef enum {
    STATE_TITLE, STATE_MAPSIZE, STATE_SEED,
    STATE_PLAYING, STATE_SAVE_MENU, STATE_LOAD_MENU
} GameState;

typedef enum {
    NPC_VILLAGER, NPC_GUARD, NPC_MERCHANT,
    NPC_INN_KEEPER, NPC_BLACKSMITH, NPC_WANDERER,
    NPC_BANDIT, NPC_WIZARD
} NPCType;

typedef enum {
    NPC_IDLE, NPC_WANDERING, NPC_TALKING,
    NPC_FOLLOWING, NPC_FLEEING, NPC_DEAD
} NPCState;

typedef enum {
    SPAWN_HOUSE, SPAWN_SHOP, SPAWN_INN,
    SPAWN_GUARD_POST, SPAWN_BANDIT_CAMP, SPAWN_TRAVELER
} SpawnType;

// ===== LOCATION TYPES =====
typedef enum {
    LOCATION_NONE = 0,
    LOCATION_VILLAGE,
    LOCATION_CASTLE,
    LOCATION_CAVE,
    LOCATION_WIZARD_TOWER,
    LOCATION_DUNGEON,
    LOCATION_RUINS,
    LOCATION_FORT,
    LOCATION_TEMPLE
    
} LocationType;

// ===== FORWARD DECLARATIONS =====
typedef struct WorldTile WorldTile;
typedef struct LocalMap LocalMap;
typedef struct NPC NPC;
typedef struct SpawnPoint SpawnPoint;

// ===== STRUCTURES =====

// Map configuration
typedef struct {
    int width;
    int height;
    const char* name;
    float defaultZoom;
} MapConfig;

// Player position
typedef struct {
    int x, y;
} Position;

// Camera system
typedef struct {
    Vector2 target;
    Vector2 offset;
    float zoom;
    float rotation;
    Camera2D camera;
} GameCamera;

// Spawn Point
struct SpawnPoint {
    int x, y;
    SpawnType type;
    NPCType npcType;
    char npcName[50];
    bool isUnique;
    int respawnTimer;
    int maxCount;
    int currentCount;
    float respawnCounter;
};

// Collision helper - defines what tiles the player can walk on
static inline bool IsTileWalkable(char tile)
{
    switch (tile) {
        // ===== TERRAIN - Always walkable =====
        case '.':  // Grass/ground
        case 'T':  // Tree/Forest
        case '^':  // Mountain/Hills
        case '~':  // Water
        case ',':  // Dirt road/path
            return true;
            
        // ===== BUILDING EXTERIORS =====
        case '#':  // Wall - NOT walkable
            return false;
            
        // ===== DOORS - Walkable (entrances/exits) =====
        case 'D':  // Door
            return true;
            
        // ===== FURNITURE - Most is NOT walkable =====
        case 'B':  // Bed - NOT walkable (you sleep on it, not walk through it)
        case 'C':  // Chair - NOT walkable (occupied)
        case 'S':  // Shelf/Counter - NOT walkable (blocks movement)
        case 'A':  // Anvil - NOT walkable (heavy equipment)
        case 'F':  // Fireplace/Fountain - NOT walkable (hazard/feature)
        case 'H':  // Chest - NOT walkable (storage)
        case 'P':  // Pillar - NOT walkable (structural)
        case 'R':  // Altar - NOT walkable (sacred object)
        case 'K':  // Throne - NOT walkable (seat of power)
        case 'Q':  // Barrel/Crate - NOT walkable (obstacle)
            return true;
            
        // ===== SPECIAL/UNKNOWN =====
        default:
            // For safety, treat unknown tiles as not walkable
            return false;
    }
}

// Add this debug version for now:
static inline bool IsTileWalkableDebug(char tile, int x, int y)
{
    if (tile == '.' || tile == 'T' || tile == '^' || 
        tile == '~' || tile == ',' || tile == 'D') {
        return true;
    }
    LOG("WARNING: Non-walkable tile '%c' at (%d,%d)\n", tile, x, y);
    return false;
}

// NPC
struct NPC {
    int id;
    char name[50];
    NPCType type;
    NPCState state;
    
    // Position
    int worldX, worldY;
    int localX, localY;
    bool isInLocalMap;
    
    // Appearance
    char displayChar;
    Color color;
    char greeting[100];
    
    // AI
    int wanderTimer;
    int wanderDirection;
    float respawnTimer;
    bool isActive;  // Add this
    
    // Interaction
    bool canTalk;
    bool canTrade;
    bool givesQuests;
    bool isUnique;
    bool isHostile;
    
    // Stats
    int health;
    int maxHealth;
    int gold;
    int relationship;
    
    // Spawn point reference
    SpawnPoint* spawnPoint;
};

// World Tile
struct WorldTile {
    char worldTile;
    LocalMap* localMap;
    bool hasLocalMap;
    LocationType locationType;
    char locationName[50];
    bool discovered;
    int lastExitX;
    int lastExitY;
    bool hasExitPoint;
};

// Player stats
typedef struct {
    Position worldPos;
    Position localPos;
    bool isInLocalMap;
} PlayerData;

// ===== GLOBAL VARIABLES =====
extern WorldTile** worldMap;
extern GameState currentState;
extern PlayerData player;
extern GameCamera gameCamera;
extern NPC** npcs;
extern int npcCount;
extern int currentMapWidth;
extern int currentMapHeight;
extern MapConfig mapSizes[NUM_SIZES];
extern bool shouldQuit;
extern int selectedOption;
extern int saveSlotSelected;

// ===== NAME DATABASE DECLARATIONS =====
extern const char* FIRST_NAMES[];
extern const char* LAST_NAMES[];
extern const int FIRST_NAME_COUNT;
extern const int LAST_NAME_COUNT;

#endif