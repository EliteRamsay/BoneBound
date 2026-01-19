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
#define MAP_WIDTH 20
#define MAP_HEIGHT 15
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

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
    STATE_PLAYING
} GameState;

// Global variables
extern char** map;
extern GameState currentState;
extern Player player;
extern int currentMapWidth;
extern int currentMapHeight;
extern MapConfig mapSizes[];
extern GameCamera gameCamera;

// Game functions
void gamestartup();
void gameupdate();
void gamedraw();
void gameshutdown();
void togglefullscreen(int windowWidth, int windowHeight);
void generate_random_map(int width, int height);
void cleanup_map();
void init_camera();
void update_camera();
void reset_camera_to_default();

// Title screen functions
void title_update();
void title_draw();

// Map size functions
void mapsize_draw();
void mapsize_update();

// Player functions
void draw_player();
void draw_hud();

// Map functions
Color get_tile_color(char tile);
void draw_map();

#endif