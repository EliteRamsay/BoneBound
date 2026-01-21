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

// Draw the world map
void draw_world_map()
{
    if (worldMap == NULL) return;
    
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
            char tile = worldMap[y][x].worldTile;
            Color tile_color = get_tile_color(tile);
            
            // Highlight tiles that have local maps
            if (worldMap[y][x].hasLocalMap) {
                if (worldMap[y][x].localMap != NULL) {
                    // Brighten visited tiles
                    tile_color.r = (tile_color.r + 30 > 255) ? 255 : tile_color.r + 30;
                    tile_color.g = (tile_color.g + 30 > 255) ? 255 : tile_color.g + 30;
                } else {
                    // Darken unvisited tiles
                    tile_color.r = (tile_color.r - 20 < 0) ? 0 : tile_color.r - 20;
                    tile_color.g = (tile_color.g - 20 < 0) ? 0 : tile_color.g - 20;
                    tile_color.b = (tile_color.b - 20 < 0) ? 0 : tile_color.b - 20;
                }
            }
            
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

// Draw local map
void draw_local_map()
{
    if (!worldMap || player.y < 0 || player.y >= currentMapHeight || 
        player.x < 0 || player.x >= currentMapWidth) return;
        
    WorldTile* currentTile = &worldMap[player.y][player.x];
    if (currentTile->localMap == NULL) return;
    
    LocalMap* local = currentTile->localMap;
    
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Calculate visible area
    float visibleWidth = screenWidth / gameCamera.camera.zoom;
    float visibleHeight = screenHeight / gameCamera.camera.zoom;
    
    Vector2 cameraWorldTopLeft = {
        gameCamera.camera.target.x - visibleWidth / 2.0f,
        gameCamera.camera.target.y - visibleHeight / 2.0f
    };
    
    // Draw black background for visible area
    DrawRectangle(cameraWorldTopLeft.x, cameraWorldTopLeft.y, 
                  visibleWidth, visibleHeight, BLACK);
    
    // Calculate visible tiles (local map coordinates)
    int startX = (int)(cameraWorldTopLeft.x / TILE_SIZE) - 1;
    int startY = (int)(cameraWorldTopLeft.y / TILE_SIZE) - 1;
    int endX = (int)((cameraWorldTopLeft.x + visibleWidth) / TILE_SIZE) + 2;
    int endY = (int)((cameraWorldTopLeft.y + visibleHeight) / TILE_SIZE) + 2;
    
    // Keep within local map bounds
    if (startX < 0) startX = 0;
    if (startY < 0) startY = 0;
    if (endX > local->width) endX = local->width;
    if (endY > local->height) endY = local->height;
    
    // Draw visible tiles
    for(int y = startY; y < endY; y++)
    {
        for(int x = startX; x < endX; x++)
        {
            char tile = local->tiles[y][x];
            Color tile_color = get_tile_color(tile);
            
            Vector2 pos = { 
                (float)(x * TILE_SIZE + 8), 
                (float)(y * TILE_SIZE + 6) 
            };
            
            int fontSize = 24;
            
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