// Globals.cpp (in root folder, not in Systems/)
#include "project.h"

// Define all global variables here
WorldTile** worldMap = NULL;
GameState currentState = STATE_TITLE;
PlayerData player = {{2, 2}, {2, 2}, false};
GameCamera gameCamera = {0};
NPC** npcs = NULL;
int npcCount = 0;
int currentMapWidth = DEFAULT_WORLD_WIDTH;
int currentMapHeight = DEFAULT_WORLD_HEIGHT;
bool shouldQuit = false;
int selectedOption = 0;
int saveSlotSelected = 0;

// Map configurations
MapConfig mapSizes[NUM_SIZES] = {
    {8, 8, "TINY", 4.0f},
    {16, 16, "SMALL", 2.0f},
    {32, 32, "MEDIUM", 1.5f},
    {64, 64, "LARGE", 0.8f},
    {128, 128, "HUGE", 0.4f},
    {256, 256, "GIGANTIC", 0.2f}
};

// Note: FIRST_NAMES, LAST_NAMES are NOT defined here
// They stay in NPC.cpp