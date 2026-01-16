#ifndef __PROJECT_H__
#define __PROJECT_H__

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#define TILE_SIZE 32
#define MAP_WIDTH 20
#define MAP_HEIGHT 15

typedef struct {
    int x, y;
} Player;

// Game states
typedef enum {
    STATE_TITLE,
    STATE_PLAYING
} GameState;

// External variables
extern char map[MAP_HEIGHT][MAP_WIDTH];
extern GameState currentState;
extern Player player;

// Game.cpp functions
void gamestartup();
void gameupdate();
void gamedraw();
void gameshutdown();
void togglefullscreen(int windowWidth, int windowHeight);
void title_update();
void title_draw();
void draw_player(Vector2 origin);


// Map.cpp functions
Color get_tile_color(char tile);
void draw_map(Vector2 origin);



#endif