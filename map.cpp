#include "project.h"

// Get color for each tile type
Color get_tile_color(char tile)
{
    switch (tile)
    {
    case '#': return GRAY;      // Wall
    case '.': return DARKGREEN; // Ground
    case '~': return BLUE;      // Water
    case '^': return BROWN;     // Mountain
    case 'T': return GREEN;     // Tree
    default: return RAYWHITE;   // Unknown
    }
}

// Draw the map
void draw_map()
{
    if (map == NULL) return;
    
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Calculate visible area
    float visibleWidth = screenWidth / gameCamera.camera.zoom;
    float visibleHeight = screenHeight / gameCamera.camera.zoom;
    
    Vector2 cameraWorldTopLeft = {
        gameCamera.camera.target.x - visibleWidth / 2.0f,
        gameCamera.camera.target.y - visibleHeight / 2.0f
    };
    
    float mapWidthWorld = currentMapWidth * TILE_SIZE;
    float mapHeightWorld = currentMapHeight * TILE_SIZE;
    
    // Draw black background for visible area
    DrawRectangle(cameraWorldTopLeft.x, cameraWorldTopLeft.y, 
                  visibleWidth, visibleHeight, BLACK);
    
    // Calculate visible tiles
    int startX = (int)(cameraWorldTopLeft.x / TILE_SIZE) - 1;
    int startY = (int)(cameraWorldTopLeft.y / TILE_SIZE) - 1;
    int endX = (int)((cameraWorldTopLeft.x + visibleWidth) / TILE_SIZE) + 2;
    int endY = (int)((cameraWorldTopLeft.y + visibleHeight) / TILE_SIZE) + 2;
    
    // Keep within map bounds
    if (startX < 0) startX = 0;
    if (startY < 0) startY = 0;
    if (endX > currentMapWidth) endX = currentMapWidth;
    if (endY > currentMapHeight) endY = currentMapHeight;
    
    // Draw visible tiles
    for(int y = startY; y < endY; y++)
    {
        for(int x = startX; x < endX; x++)
        {
            char tile = map[y][x];
            Color tile_color = get_tile_color(tile);
            
            Vector2 pos = { 
                (float)(x * TILE_SIZE + 8), 
                (float)(y * TILE_SIZE + 6) 
            };
            
            // Larger text for small maps
            int fontSize = (currentMapWidth <= 8 && currentMapHeight <= 8) ? 28 : 24;
            
            DrawTextEx(
                GetFontDefault(),
                TextFormat("%c", tile),
                pos,
                fontSize,
                1,
                tile_color);
        }
    }
}