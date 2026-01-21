#ifndef __PROJECT_H__
#define __PROJECT_H__

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

// Game constants
#define TILE_SIZE 32
#define LOCAL_MAP_WIDTH 256  // Changed to 256x256 as requested
#define LOCAL_MAP_HEIGHT 256
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Default world map size
#define DEFAULT_WORLD_WIDTH 20
#define DEFAULT_WORLD_HEIGHT 15

// Map size types
typedef enum {
    SIZE_TINY,
    SIZE_SMALL,
    SIZE_MEDIUM,
    SIZE_LARGE,
    SIZE_HUGE,
    SIZE_GIGANTIC,
    NUM_SIZES
} MapSize;

// Local map structure
typedef struct {
    char** tiles;
    int width;
    int height;
} LocalMap;

// World map tile
typedef struct {
    char worldTile;
    LocalMap* localMap;
    bool hasLocalMap;
} WorldTile;

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
} Player;

// Camera for following player
typedef struct {
    Vector2 target;
    Vector2 offset;
    float zoom;
    Camera2D camera;
} GameCamera;

// Game states
typedef enum {
    STATE_TITLE,
    STATE_MAPSIZE,
    STATE_SEED,
    STATE_PLAYING,
    STATE_SAVE_MENU,
    STATE_LOAD_MENU
} GameState;

// Global variables
extern WorldTile** worldMap;
extern GameState currentState;
extern Player player;
extern Player localPlayer;
extern int currentMapWidth;
extern int currentMapHeight;
extern MapConfig mapSizes[];
extern GameCamera gameCamera;
extern bool isInLocalMap;
extern int saveSlotSelected;
extern bool shouldQuit;  // Add quit flag

// Game functions
void gamestartup();
void gameupdate();
void gamedraw();
void gameshutdown();
void togglefullscreen(int windowWidth, int windowHeight);
void generate_world_map(int width, int height);
void cleanup_all_maps();
void init_camera();
void update_camera();
void reset_camera_to_default();

// Title screen functions
void title_update();
void title_draw();

// Map size functions
void mapsize_draw();
void mapsize_update();

// Save/Load functions
void save_game_to_slot(int slot);
bool load_game_from_slot(int slot);
void save_menu_draw();
void save_menu_update();
void load_menu_draw();
void load_menu_update();
bool save_file_exists(int slot);  // New function

// Player functions
void draw_player();
void draw_hud();
void draw_local_player();

// Map functions
Color get_tile_color(char tile);
void draw_world_map();
void draw_local_map();

// Local map functions
void enter_local_map(int worldX, int worldY);
void exit_local_map();
void generate_local_map_at(int worldX, int worldY);


#endif