#include "project.h"

// Global game variables
WorldTile** worldMap = NULL;
Player player;
Player localPlayer;
GameState currentState = STATE_TITLE;
GameCamera gameCamera;
bool isInLocalMap = false;
int saveSlotSelected = 0;
bool shouldQuit = false;  // Quit flag

// Menu variables
int selectedOption = 0;
bool hasSave = false;

// Map data
int currentMapWidth = DEFAULT_WORLD_WIDTH;
int currentMapHeight = DEFAULT_WORLD_HEIGHT;

// Map sizes with default zoom levels
MapConfig mapSizes[NUM_SIZES] = {
    {8, 8, "TINY", 4.0f},
    {16, 16, "SMALL", 2.0f},
    {32, 32, "MEDIUM", 1.5f},
    {64, 64, "LARGE", 0.8f},
    {128, 128, "HUGE", 0.4f},
    {256, 256, "GIGANTIC", 0.2f}
};

// Check if save file exists
bool save_file_exists(int slot)
{
    char filename[50];
    sprintf(filename, "save_%d.dat", slot);
    FILE* test = fopen(filename, "rb");
    if (test) {
        fclose(test);
        return true;
    }
    return false;
}

// Clamp a value between min and max
float clamp_float(float value, float min, float max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

// Set up camera
void init_camera()
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Center camera on player
    gameCamera.camera.target = (Vector2){ 
        (float)(player.x * TILE_SIZE + TILE_SIZE / 2), 
        (float)(player.y * TILE_SIZE + TILE_SIZE / 2) 
    };
    
    // Offset is screen center
    gameCamera.camera.offset = (Vector2){ 
        (float)screenWidth / 2.0f, 
        (float)screenHeight / 2.0f 
    };
    
    gameCamera.camera.rotation = 0.0f;
    
    // Set zoom based on map size
    gameCamera.zoom = 1.0f; // Default
    for (int i = 0; i < NUM_SIZES; i++) {
        if (currentMapWidth == mapSizes[i].width && currentMapHeight == mapSizes[i].height) {
            gameCamera.zoom = mapSizes[i].defaultZoom;
            break;
        }
    }
    
    gameCamera.camera.zoom = gameCamera.zoom;
    
    // Center small maps on screen
    if (currentMapWidth <= 16 && currentMapHeight <= 16) {
        gameCamera.camera.target = (Vector2){ 
            (float)(currentMapWidth * TILE_SIZE) / 2.0f,
            (float)(currentMapHeight * TILE_SIZE) / 2.0f
        };
    }
}

// Reset camera to defaults
void reset_camera_to_default()
{
    init_camera();
}

// Update camera to follow player
void update_camera()
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Update camera offset
    gameCamera.camera.offset = (Vector2){ 
        (float)screenWidth / 2.0f, 
        (float)screenHeight / 2.0f 
    };
    
    // Target is player position
    Vector2 targetPos;
    if (isInLocalMap) {
        targetPos = { 
            (float)(localPlayer.x * TILE_SIZE + TILE_SIZE / 2.0f), 
            (float)(localPlayer.y * TILE_SIZE + TILE_SIZE / 2.0f) 
        };
    } else {
        targetPos = { 
            (float)(player.x * TILE_SIZE + TILE_SIZE / 2.0f), 
            (float)(player.y * TILE_SIZE + TILE_SIZE / 2.0f) 
        };
    }
    
    // Smooth camera movement
    float lerpSpeed = 0.15f;
    gameCamera.camera.target.x += (targetPos.x - gameCamera.camera.target.x) * lerpSpeed;
    gameCamera.camera.target.y += (targetPos.y - gameCamera.camera.target.y) * lerpSpeed;
    
    float mapWidthWorld, mapHeightWorld;
    if (isInLocalMap && worldMap[player.y][player.x].localMap != NULL) {
        LocalMap* local = worldMap[player.y][player.x].localMap;
        mapWidthWorld = local->width * TILE_SIZE;
        mapHeightWorld = local->height * TILE_SIZE;
    } else {
        mapWidthWorld = currentMapWidth * TILE_SIZE;
        mapHeightWorld = currentMapHeight * TILE_SIZE;
    }
    
    // Handle zoom with mouse wheel
    float wheelMove = GetMouseWheelMove();
    if (wheelMove != 0) {
        float zoomChange = wheelMove * 0.1f * gameCamera.zoom;
        float newZoom = gameCamera.zoom + zoomChange;
        
        // Set zoom limits based on map size
        float minZoom, maxZoom;
        
        if (isInLocalMap) {
            minZoom = 0.1f;
            maxZoom = 2.0f;
        } else if (currentMapWidth <= 8 && currentMapHeight <= 8) {
            minZoom = 1.0f;
            maxZoom = 8.0f;
        } else if (currentMapWidth <= 16 && currentMapHeight <= 16) {
            minZoom = 0.5f;
            maxZoom = 6.0f;
        } else if (currentMapWidth <= 32 && currentMapHeight <= 32) {
            minZoom = 0.3f;
            maxZoom = 4.0f;
        } else {
            minZoom = 0.1f;
            maxZoom = 3.0f;
        }
        
        newZoom = clamp_float(newZoom, minZoom, maxZoom);
        
        if (newZoom != gameCamera.zoom) {
            gameCamera.zoom = newZoom;
            gameCamera.camera.zoom = gameCamera.zoom;
        }
    }
    
    // Clamp camera to map boundaries
    float visibleWidth = screenWidth / gameCamera.zoom;
    float visibleHeight = screenHeight / gameCamera.zoom;
    
    if (mapWidthWorld < visibleWidth && mapHeightWorld < visibleHeight) {
        // Map fits on screen - center it
        gameCamera.camera.target.x = mapWidthWorld / 2.0f;
        gameCamera.camera.target.y = mapHeightWorld / 2.0f;
    } else {
        // Map larger than screen - clamp to edges
        float minX = visibleWidth / 2.0f;
        float maxX = mapWidthWorld - visibleWidth / 2.0f;
        float minY = visibleHeight / 2.0f;
        float maxY = mapHeightWorld - visibleHeight / 2.0f;
        
        if (maxX > minX) {
            gameCamera.camera.target.x = clamp_float(gameCamera.camera.target.x, minX, maxX);
        } else {
            gameCamera.camera.target.x = mapWidthWorld / 2.0f;
        }
        
        if (maxY > minY) {
            gameCamera.camera.target.y = clamp_float(gameCamera.camera.target.y, minY, maxY);
        } else {
            gameCamera.camera.target.y = mapHeightWorld / 2.0f;
        }
    }
}

// Initialize game
void gamestartup()
{
    InitAudioDevice();
    player = {2, 2};
    localPlayer = {2, 2};
    currentState = STATE_TITLE;
    selectedOption = 0;
    hasSave = false;
    isInLocalMap = false;
    shouldQuit = false;
}

// Toggle fullscreen mode
void togglefullscreen(int windowWidth, int windowHeight)
{
    if(!IsWindowFullscreen())
    {
        int monitor = GetCurrentMonitor();
        SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
        ToggleFullscreen();
        
        if (currentState == STATE_PLAYING)
        {
            gameCamera.camera.offset = (Vector2){ 
                (float)GetScreenWidth() / 2.0f, 
                (float)GetScreenHeight() / 2.0f 
            };
        }
    }
    else
    {
        ToggleFullscreen();
        SetWindowSize(windowWidth, windowHeight);
        
        if (currentState == STATE_PLAYING)
        {
            gameCamera.camera.offset = (Vector2){ 
                (float)GetScreenWidth() / 2.0f, 
                (float)GetScreenHeight() / 2.0f 
            };
        }
    }
}

// Create world map
void generate_world_map(int width, int height)
{
    cleanup_all_maps();
    
    currentMapWidth = width;
    currentMapHeight = height;
    
    // Allocate world map memory
    worldMap = (WorldTile**)malloc(height * sizeof(WorldTile*));
    for (int y = 0; y < height; y++)
    {
        worldMap[y] = (WorldTile*)malloc(width * sizeof(WorldTile));
        
        for (int x = 0; x < width; x++)
        {
            // Border walls
            if (x == 0 || x == width - 1 || y == 0 || y == height - 1)
            {
                worldMap[y][x].worldTile = '#';
                worldMap[y][x].hasLocalMap = false;
            }
            else
            {
                // Random terrain
                float randVal = (float)rand() / RAND_MAX;
                
                if (randVal < 0.05) {
                    worldMap[y][x].worldTile = '~';
                    worldMap[y][x].hasLocalMap = true;  // Water can have local maps
                }
                else if (randVal < 0.15) {
                    worldMap[y][x].worldTile = '^';
                    worldMap[y][x].hasLocalMap = true;  // Mountains can have local maps
                }
                else if (randVal < 0.20) {
                    worldMap[y][x].worldTile = 'T';
                    worldMap[y][x].hasLocalMap = true;  // Forests can have local maps
                }
                else {
                    worldMap[y][x].worldTile = '.';
                    worldMap[y][x].hasLocalMap = true;  // Grasslands can have local maps
                }
            }
            
            worldMap[y][x].localMap = NULL; // Not generated yet
        }
    }
    
    // Clear starting area
    for (int y = 1; y <= 3; y++)
    {
        for (int x = 1; x <= 3; x++)
        {
            if (y < height && x < width) {
                worldMap[y][x].worldTile = '.';
                worldMap[y][x].hasLocalMap = true;
            }
        }
    }
    
    // Set player start
    player.x = 2;
    player.y = 2;
    localPlayer.x = 2;
    localPlayer.y = 2;
    
    // Setup camera
    init_camera();
}

// Generate local map at specific world coordinates
void generate_local_map_at(int worldX, int worldY)
{
    if (!worldMap[worldY][worldX].hasLocalMap) return;
    
    // Allocate local map
    LocalMap* local = (LocalMap*)malloc(sizeof(LocalMap));
    local->width = LOCAL_MAP_WIDTH;   // 256 as requested
    local->height = LOCAL_MAP_HEIGHT; // 256 as requested
    
    // Allocate tiles
    local->tiles = (char**)malloc(local->height * sizeof(char*));
    for (int y = 0; y < local->height; y++)
    {
        local->tiles[y] = (char*)malloc((local->width + 1) * sizeof(char));
        
        for (int x = 0; x < local->width; x++)
        {
            // Border walls
            if (x == 0 || x == local->width - 1 || y == 0 || y == local->height - 1)
            {
                local->tiles[y][x] = '#';
            }
            else
            {
                // Generate based on world tile type with variety
                switch (worldMap[worldY][worldX].worldTile)
                {
                    case '.':  // Grassland
                        {
                            float randVal = (float)rand() / RAND_MAX;
                            if (randVal < 0.02) local->tiles[y][x] = '~';      // Some water
                            else if (randVal < 0.04) local->tiles[y][x] = '^'; // Some mountains
                            else if (randVal < 0.10) local->tiles[y][x] = 'T'; // Some trees
                            else local->tiles[y][x] = '.';
                        }
                        break;
                    case 'T':  // Forest
                        {
                            float randVal = (float)rand() / RAND_MAX;
                            if (randVal < 0.01) local->tiles[y][x] = '~';      // Some water
                            else if (randVal < 0.02) local->tiles[y][x] = '^'; // Some mountains
                            else if (randVal < 0.70) local->tiles[y][x] = 'T'; // Mostly trees
                            else local->tiles[y][x] = '.';
                        }
                        break;
                    case '~':  // Water
                        {
                            float randVal = (float)rand() / RAND_MAX;
                            if (randVal < 0.90) local->tiles[y][x] = '~';      // Mostly water
                            else if (randVal < 0.95) local->tiles[y][x] = '.'; // Some land
                            else local->tiles[y][x] = '^';                     // Some mountains in water
                        }
                        break;
                    case '^':  // Mountains
                        {
                            float randVal = (float)rand() / RAND_MAX;
                            if (randVal < 0.85) local->tiles[y][x] = '^';      // Mostly mountains
                            else if (randVal < 0.90) local->tiles[y][x] = '~'; // Some water
                            else local->tiles[y][x] = '.';                     // Some clear areas
                        }
                        break;
                    default:
                        local->tiles[y][x] = '.';
                }
            }
        }
        local->tiles[y][local->width] = '\0';
    }
    
    // Clear starting area in local map
    for (int y = 1; y <= 3; y++)
    {
        for (int x = 1; x <= 3; x++)
        {
            if (y < local->height && x < local->width)
                local->tiles[y][x] = '.';
        }
    }
    
    // Set to world map
    worldMap[worldY][worldX].localMap = local;
}

// Enter local map
void enter_local_map(int worldX, int worldY)
{
    if (!worldMap[worldY][worldX].hasLocalMap) return;
    
    // Generate if not exists
    if (worldMap[worldY][worldX].localMap == NULL)
    {
        generate_local_map_at(worldX, worldY);
    }
    
    // Switch to local map state
    isInLocalMap = true;
    localPlayer.x = LOCAL_MAP_WIDTH / 2;
    localPlayer.y = LOCAL_MAP_HEIGHT / 2;
    
    // Adjust camera for local map
    reset_camera_to_default();
}

// Exit local map
void exit_local_map()
{
    isInLocalMap = false;
    reset_camera_to_default();
}

// Free all map memory
void cleanup_all_maps()
{
    if (worldMap != NULL)
    {
        for (int y = 0; y < currentMapHeight; y++)
        {
            for (int x = 0; x < currentMapWidth; x++)
            {
                if (worldMap[y][x].localMap != NULL)
                {
                    LocalMap* local = worldMap[y][x].localMap;
                    for (int ly = 0; ly < local->height; ly++)
                    {
                        free(local->tiles[ly]);
                    }
                    free(local->tiles);
                    free(local);
                }
            }
            free(worldMap[y]);
        }
        free(worldMap);
        worldMap = NULL;
    }
}

// Save game to slot
void save_game_to_slot(int slot)
{
    char filename[50];
    sprintf(filename, "save_%d.dat", slot);
    
    FILE* file = fopen(filename, "wb");
    if (!file) return;
    
    // Save world dimensions
    fwrite(&currentMapWidth, sizeof(int), 1, file);
    fwrite(&currentMapHeight, sizeof(int), 1, file);
    
    // Save player position
    fwrite(&player.x, sizeof(int), 1, file);
    fwrite(&player.y, sizeof(int), 1, file);
    fwrite(&localPlayer.x, sizeof(int), 1, file);
    fwrite(&localPlayer.y, sizeof(int), 1, file);
    fwrite(&isInLocalMap, sizeof(bool), 1, file);
    
    // Save world map
    for (int y = 0; y < currentMapHeight; y++)
    {
        for (int x = 0; x < currentMapWidth; x++)
        {
            // Save world tile data
            fwrite(&worldMap[y][x].worldTile, sizeof(char), 1, file);
            fwrite(&worldMap[y][x].hasLocalMap, sizeof(bool), 1, file);
            
            // Save local map if exists
            bool hasLocal = (worldMap[y][x].localMap != NULL);
            fwrite(&hasLocal, sizeof(bool), 1, file);
            
            if (hasLocal)
            {
                LocalMap* local = worldMap[y][x].localMap;
                fwrite(&local->width, sizeof(int), 1, file);
                fwrite(&local->height, sizeof(int), 1, file);
                
                for (int ly = 0; ly < local->height; ly++)
                {
                    fwrite(local->tiles[ly], sizeof(char), local->width, file);
                }
            }
        }
    }
    
    fclose(file);
}

// Load game from slot
bool load_game_from_slot(int slot)
{
    char filename[50];
    sprintf(filename, "save_%d.dat", slot);
    
    FILE* file = fopen(filename, "rb");
    if (!file) return false;
    
    // Clean up existing maps
    cleanup_all_maps();
    
    // Load world dimensions
    fread(&currentMapWidth, sizeof(int), 1, file);
    fread(&currentMapHeight, sizeof(int), 1, file);
    
    // Load player position
    fread(&player.x, sizeof(int), 1, file);
    fread(&player.y, sizeof(int), 1, file);
    fread(&localPlayer.x, sizeof(int), 1, file);
    fread(&localPlayer.y, sizeof(int), 1, file);
    fread(&isInLocalMap, sizeof(bool), 1, file);
    
    // Allocate world map
    worldMap = (WorldTile**)malloc(currentMapHeight * sizeof(WorldTile*));
    for (int y = 0; y < currentMapHeight; y++)
    {
        worldMap[y] = (WorldTile*)malloc(currentMapWidth * sizeof(WorldTile));
        
        for (int x = 0; x < currentMapWidth; x++)
        {
            // Load world tile data
            fread(&worldMap[y][x].worldTile, sizeof(char), 1, file);
            fread(&worldMap[y][x].hasLocalMap, sizeof(bool), 1, file);
            
            // Load local map if exists
            bool hasLocal;
            fread(&hasLocal, sizeof(bool), 1, file);
            
            if (hasLocal)
            {
                LocalMap* local = (LocalMap*)malloc(sizeof(LocalMap));
                fread(&local->width, sizeof(int), 1, file);
                fread(&local->height, sizeof(int), 1, file);
                
                local->tiles = (char**)malloc(local->height * sizeof(char*));
                for (int ly = 0; ly < local->height; ly++)
                {
                    local->tiles[ly] = (char*)malloc((local->width + 1) * sizeof(char));
                    fread(local->tiles[ly], sizeof(char), local->width, file);
                    local->tiles[ly][local->width] = '\0';
                }
                
                worldMap[y][x].localMap = local;
            }
            else
            {
                worldMap[y][x].localMap = NULL;
            }
        }
    }
    
    fclose(file);
    
    // Setup camera
    init_camera();
    currentState = STATE_PLAYING;
    
    return true;
}

// Update game logic
void gameupdate()
{
    if (shouldQuit) {
        CloseWindow(); // This will break the main loop
        return;
    }
    
    if (currentState == STATE_TITLE) 
    {
        if(IsKeyPressed(KEY_F))
        {
            togglefullscreen(SCREEN_WIDTH, SCREEN_HEIGHT);
        }
        title_update();
    } 
    else if (currentState == STATE_MAPSIZE) 
    {
        if(IsKeyPressed(KEY_F))
        {
            togglefullscreen(SCREEN_WIDTH, SCREEN_HEIGHT);
        }
        mapsize_update();
    }
    else if (currentState == STATE_SAVE_MENU)
    {
        save_menu_update();
    }
    else if (currentState == STATE_LOAD_MENU)
    {
        load_menu_update();
    }
    else if (currentState == STATE_PLAYING) 
    {
        if (isInLocalMap)
        {
            // Inside local map
            LocalMap* currentLocal = worldMap[player.y][player.x].localMap;
            
            // Store old position
            int oldX = localPlayer.x;
            int oldY = localPlayer.y;
            
            // Player movement within local map
            if ((IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) && 
                localPlayer.x + 1 < currentLocal->width && 
                currentLocal->tiles[localPlayer.y][localPlayer.x + 1] != '#') localPlayer.x++;
            
            if ((IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) && 
                localPlayer.x > 0 && 
                currentLocal->tiles[localPlayer.y][localPlayer.x - 1] != '#') localPlayer.x--;
            
            if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && 
                localPlayer.y > 0 && 
                currentLocal->tiles[localPlayer.y - 1][localPlayer.x] != '#') localPlayer.y--;
            
            if ((IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) && 
                localPlayer.y + 1 < currentLocal->height && 
                currentLocal->tiles[localPlayer.y + 1][localPlayer.x] != '#') localPlayer.y++;
            
            // Exit local map with BACKSPACE only (not at edges)
            if (IsKeyPressed(KEY_BACKSPACE))
            {
                exit_local_map();
            }
            
            // Update camera if player moved
            if (oldX != localPlayer.x || oldY != localPlayer.y)
            {
                update_camera();
            }
        }
        else
        {
            // On world map
            int oldX = player.x;
            int oldY = player.y;
            
            // Player movement on world map
            if ((IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) && 
                player.x + 1 < currentMapWidth && 
                worldMap[player.y][player.x + 1].worldTile != '#') player.x++;
            
            if ((IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) && 
                player.x > 0 && 
                worldMap[player.y][player.x - 1].worldTile != '#') player.x--;
            
            if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && 
                player.y > 0 && 
                worldMap[player.y - 1][player.x].worldTile != '#') player.y--;
            
            if ((IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) && 
                player.y + 1 < currentMapHeight && 
                worldMap[player.y + 1][player.x].worldTile != '#') player.y++;
            
            // Enter local map
            if (IsKeyPressed(KEY_ENTER) && worldMap[player.y][player.x].hasLocalMap)
            {
                enter_local_map(player.x, player.y);
            }
            
            // Update camera if player moved
            if (oldX != player.x || oldY != player.y)
            {
                update_camera();
            }
            
            // Return to menu with BACKSPACE
            if (IsKeyPressed(KEY_BACKSPACE))
            {
                currentState = STATE_TITLE;
                selectedOption = 0;
            }
        }
        
        // Always update camera
        update_camera();
        
        // Toggle fullscreen
        if(IsKeyPressed(KEY_F))
        {
            int currentWidth = GetScreenWidth();
            int currentHeight = GetScreenHeight();
            togglefullscreen(currentWidth, currentHeight);
            update_camera();
        }
        
        // Reset camera
        if (IsKeyPressed(KEY_R))
        {
            reset_camera_to_default();
        }

        // Save menu
        if (IsKeyPressed(KEY_F5))
        {
            currentState = STATE_SAVE_MENU;
            saveSlotSelected = 0;
        }
        
        // Load menu
        if (IsKeyPressed(KEY_F9))
        {
            currentState = STATE_LOAD_MENU;
            saveSlotSelected = 0;
        }
    }
}

// Draw game
void gamedraw()
{
    BeginDrawing();
    ClearBackground(BLACK);
    
    if (currentState == STATE_TITLE) 
    {
        title_draw();
    }
    else if (currentState == STATE_MAPSIZE) 
    {
        mapsize_draw();
    } 
    else if (currentState == STATE_SAVE_MENU)
    {
        save_menu_draw();
    }
    else if (currentState == STATE_LOAD_MENU)
    {
        load_menu_draw();
    }
    else if (currentState == STATE_PLAYING) 
    {
        BeginMode2D(gameCamera.camera);
        
        if (isInLocalMap)
        {
            // Draw local map and player
            draw_local_map();
            draw_local_player();
        }
        else
        {
            // Draw world map and player
            draw_world_map();
            draw_player();
        }
        
        EndMode2D();
        
        // Draw HUD
        draw_hud();
    }
    
    EndDrawing();
}

// Clean up game
void gameshutdown()
{
    cleanup_all_maps();
    CloseAudioDevice();
}

// Update title screen
void title_update()
{
    // Menu navigation
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        selectedOption = (selectedOption + 1) % 3;
    }
    
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        selectedOption = (selectedOption - 1 + 3) % 3;
    }
    
    // Menu selection
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        if (selectedOption == 0) {
            selectedOption = 0;
            currentState = STATE_MAPSIZE;
        } else if (selectedOption == 1) {
            // Only go to load menu if a save exists
            if (save_file_exists(0) || save_file_exists(1) || save_file_exists(2)) {
                currentState = STATE_LOAD_MENU;
                saveSlotSelected = 0;
            }
        } else if (selectedOption == 2) {
            // Quit game
            shouldQuit = true;
        }
    }
}

// Draw title screen
void title_draw()
{
    const char* gameTitle = "BoneBound";
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Draw title
    Vector2 titleSize = MeasureTextEx(GetFontDefault(), gameTitle, 48, 2);
    Vector2 titlePos = {
        (float)screenWidth / 2.0f - titleSize.x / 2.0f,
        (float)screenHeight / 4.0f
    };
    DrawTextEx(GetFontDefault(), gameTitle, titlePos, 48, 2, YELLOW);
    
    // Draw menu options
    const char* options[] = {"NEW GAME", "LOAD GAME", "QUIT"};
    
    for (int i = 0; i < 3; i++) {
        Color color = (i == selectedOption) ? YELLOW : WHITE;
        
        // Gray out LOAD GAME if no saves exist
        if (i == 1 && !(save_file_exists(0) || save_file_exists(1) || save_file_exists(2))) {
            color = GRAY;
        }
        
        const char* optionText = options[i];
        Vector2 textSize = MeasureTextEx(GetFontDefault(), optionText, 32, 1);
        Vector2 textPos = {
            (float)screenWidth / 2.0f - textSize.x / 2.0f,
            (float)screenHeight / 2.0f + i * 50.0f
        };
        
        // Selection arrows
        if (i == selectedOption && !(i == 1 && color.r == GRAY.r && color.g == GRAY.g && color.b == GRAY.b)) {
            DrawText(">", (int)textPos.x - 21, (int)textPos.y, 32, YELLOW);
            DrawText("<", (int)(textPos.x + textSize.x + 10), (int)textPos.y, 32, YELLOW);
        }
        
        DrawTextEx(GetFontDefault(), optionText, textPos, 32, 1, color);
    }
    
    // Instructions
    const char* instructions = "Use UP/DOWN to navigate, ENTER to select";
    Vector2 instSize = MeasureTextEx(GetFontDefault(), instructions, 20, 1);
    Vector2 instPos = {
        (float)screenWidth / 2.0f - instSize.x / 2.0f,
        (float)screenHeight - 50.0f
    };
    DrawTextEx(GetFontDefault(), instructions, instPos, 20, 1, LIGHTGRAY);
}

// Update map size selection
void mapsize_update()
{
    // Navigation
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        selectedOption = (selectedOption + 1) % NUM_SIZES;
    }
    
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        selectedOption = (selectedOption - 1 + NUM_SIZES) % NUM_SIZES;
    }
    
    // Selection
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        generate_world_map(mapSizes[selectedOption].width, mapSizes[selectedOption].height);
        currentState = STATE_PLAYING;
    }
    
    // Back to menu
    if (IsKeyPressed(KEY_BACKSPACE)) {
        currentState = STATE_TITLE;
    }
}

// Draw map size selection
void mapsize_draw()
{
    const char* Title = "Map Size";
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Title
    Vector2 titleSize = MeasureTextEx(GetFontDefault(), Title, 48, 2);
    Vector2 titlePos = {
        (float)screenWidth / 2.0f - titleSize.x / 2.0f,
        (float)screenHeight / 6.0f
    };
    DrawTextEx(GetFontDefault(), Title, titlePos, 48, 2, YELLOW);
    
    // Selected map info
    const char* infoText = TextFormat("Selected: %s (%dx%d)", 
        mapSizes[selectedOption].name, 
        mapSizes[selectedOption].width, 
        mapSizes[selectedOption].height);
    
    Vector2 infoSize = MeasureTextEx(GetFontDefault(), infoText, 22, 1);
    Vector2 infoPos = {
        (float)screenWidth / 2.0f - infoSize.x / 2.0f,
        (float)screenHeight / 3.0f
    };
    DrawTextEx(GetFontDefault(), infoText, infoPos, 22, 1, LIGHTGRAY);
    
    // Map size options
    const char* options[] = {"TINY", "SMALL", "MEDIUM", "LARGE", "HUGE", "GIGANTIC"};
    
    for (int i = 0; i < NUM_SIZES; i++) {
        Color color = (i == selectedOption) ? YELLOW : WHITE;
        
        char optionText[50];
        sprintf(optionText, "%s (%dx%d)", options[i], mapSizes[i].width, mapSizes[i].height);
        
        Vector2 textSize = MeasureTextEx(GetFontDefault(), optionText, 28, 1);
        Vector2 textPos = {
            (float)screenWidth / 2.0f - textSize.x / 2.0f,
            (float)screenHeight / 2.0f + i * 40.0f
        };
        
        // Selection arrows
        if (i == selectedOption) {
            DrawText(">", (int)textPos.x - 21, (int)textPos.y, 28, YELLOW);
            DrawText("<", (int)(textPos.x + textSize.x + 10), (int)textPos.y, 28, YELLOW);
        }
        
        DrawTextEx(GetFontDefault(), optionText, textPos, 28, 1, color);
    }
    
    // Instructions
    const char* instructions[] = {
        "Use UP/DOWN to navigate",
        "ENTER to select map size",
        "BACKSPACE to return to menu"
    };
    
    for (int i = 0; i < 3; i++) {
        Vector2 instSize = MeasureTextEx(GetFontDefault(), instructions[i], 20, 1);
        Vector2 instPos = {
            (float)screenWidth / 2.0f - instSize.x / 2.0f,
            (float)screenHeight - 100.0f + i * 25.0f
        };
        DrawTextEx(GetFontDefault(), instructions[i], instPos, 20, 1, LIGHTGRAY);
    }
}

// Save menu update
void save_menu_update()
{
    // Navigation
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        saveSlotSelected = (saveSlotSelected + 1) % 3;
    }
    
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        saveSlotSelected = (saveSlotSelected - 1 + 3) % 3;
    }
    
    // Selection
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        save_game_to_slot(saveSlotSelected);
        currentState = STATE_PLAYING;
    }
    
    // Back to game
    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_ESCAPE)) {
        currentState = STATE_PLAYING;
    }
}

// Save menu draw
void save_menu_draw()
{
    const char* Title = "Save Game";
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Title
    Vector2 titleSize = MeasureTextEx(GetFontDefault(), Title, 48, 2);
    Vector2 titlePos = {
        (float)screenWidth / 2.0f - titleSize.x / 2.0f,
        (float)screenHeight / 6.0f
    };
    DrawTextEx(GetFontDefault(), Title, titlePos, 48, 2, YELLOW);
    
    // Save slots
    const char* slotNames[] = {"Save Slot 1", "Save Slot 2", "Save Slot 3"};
    
    for (int i = 0; i < 3; i++) {
        Color color = (i == saveSlotSelected) ? YELLOW : WHITE;
        
        Vector2 textSize = MeasureTextEx(GetFontDefault(), slotNames[i], 32, 1);
        Vector2 textPos = {
            (float)screenWidth / 2.0f - textSize.x / 2.0f,
            (float)screenHeight / 2.0f + i * 50.0f
        };
        
        // Selection arrows
        if (i == saveSlotSelected) {
            DrawText(">", (int)textPos.x - 21, (int)textPos.y, 32, YELLOW);
            DrawText("<", (int)(textPos.x + textSize.x + 10), (int)textPos.y, 32, YELLOW);
        }
        
        DrawTextEx(GetFontDefault(), slotNames[i], textPos, 32, 1, color);
    }
    
    // Instructions
    const char* instructions[] = {
        "Use UP/DOWN to select save slot",
        "ENTER to save",
        "BACKSPACE to return to game"
    };
    
    for (int i = 0; i < 3; i++) {
        Vector2 instSize = MeasureTextEx(GetFontDefault(), instructions[i], 20, 1);
        Vector2 instPos = {
            (float)screenWidth / 2.0f - instSize.x / 2.0f,
            (float)screenHeight - 100.0f + i * 25.0f
        };
        DrawTextEx(GetFontDefault(), instructions[i], instPos, 20, 1, LIGHTGRAY);
    }
}

// Load menu update
void load_menu_update()
{
    // Navigation
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        saveSlotSelected = (saveSlotSelected + 1) % 3;
    }
    
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        saveSlotSelected = (saveSlotSelected - 1 + 3) % 3;
    }
    
    // Selection - only if save exists
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        if (save_file_exists(saveSlotSelected)) {
            if (load_game_from_slot(saveSlotSelected)) {
                currentState = STATE_PLAYING;
            }
        }
    }
    
    // Back to game
    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_ESCAPE)) {
        currentState = STATE_PLAYING;
    }
}

// Load menu draw
void load_menu_draw()
{
    const char* Title = "Load Game";
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Title
    Vector2 titleSize = MeasureTextEx(GetFontDefault(), Title, 48, 2);
    Vector2 titlePos = {
        (float)screenWidth / 2.0f - titleSize.x / 2.0f,
        (float)screenHeight / 6.0f
    };
    DrawTextEx(GetFontDefault(), Title, titlePos, 48, 2, YELLOW);
    
    // Load slots
    const char* slotNames[] = {"Load Slot 1", "Load Slot 2", "Load Slot 3"};
    
    for (int i = 0; i < 3; i++) {
        Color color = (i == saveSlotSelected) ? YELLOW : WHITE;
        
        // Check if save file exists
        bool exists = save_file_exists(i);
        
        if (!exists) color = GRAY;
        
        Vector2 textSize = MeasureTextEx(GetFontDefault(), slotNames[i], 32, 1);
        Vector2 textPos = {
            (float)screenWidth / 2.0f - textSize.x / 2.0f,
            (float)screenHeight / 2.0f + i * 50.0f
        };
        
        // Selection arrows - only if save exists
        if (i == saveSlotSelected && exists) {
            DrawText(">", (int)textPos.x - 21, (int)textPos.y, 32, YELLOW);
            DrawText("<", (int)(textPos.x + textSize.x + 10), (int)textPos.y, 32, YELLOW);
        }
        
        DrawTextEx(GetFontDefault(), slotNames[i], textPos, 32, 1, color);
        
        // Show if empty
        if (!exists) {
            Vector2 emptySize = MeasureTextEx(GetFontDefault(), "(Empty)", 20, 1);
            Vector2 emptyPos = {
                (float)screenWidth / 2.0f - emptySize.x / 2.0f,
                textPos.y + 35.0f
            };
            DrawTextEx(GetFontDefault(), "(Empty)", emptyPos, 20, 1, GRAY);
        }
    }
    
    // Instructions
    const char* instructions[] = {
        "Use UP/DOWN to select save slot",
        "ENTER to load (if save exists)",
        "BACKSPACE to return to game"
    };
    
    for (int i = 0; i < 3; i++) {
        Vector2 instSize = MeasureTextEx(GetFontDefault(), instructions[i], 20, 1);
        Vector2 instPos = {
            (float)screenWidth / 2.0f - instSize.x / 2.0f,
            (float)screenHeight - 100.0f + i * 25.0f
        };
        DrawTextEx(GetFontDefault(), instructions[i], instPos, 20, 1, LIGHTGRAY);
    }
}

// Draw player (on world map)
void draw_player()
{
    Vector2 pos = 
    {
        (float)(player.x * TILE_SIZE + 8), 
        (float)(player.y * TILE_SIZE + 6)
    };
    
    DrawTextEx(
        GetFontDefault(),
        "D",
        pos,
        24,
        1,
        YELLOW);
}

// Draw local player (inside local map)
void draw_local_player()
{
    Vector2 pos = 
    {
        (float)(localPlayer.x * TILE_SIZE + 8), 
        (float)(localPlayer.y * TILE_SIZE + 6)
    };
    
    DrawTextEx(
        GetFontDefault(),
        "D",
        pos,
        24,
        1,
        YELLOW);
}

// Draw HUD
void draw_hud()
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    if (isInLocalMap)
    {
        DrawText("Local Map - BACKSPACE: Exit to World | F5: Save | F9: Load", 10, screenHeight - 30, 18, LIGHTGRAY);
        DrawText(TextFormat("Local Position: %d,%d | Map: %dx%d", localPlayer.x, localPlayer.y, LOCAL_MAP_WIDTH, LOCAL_MAP_HEIGHT), 
                10, screenHeight - 55, 18, LIGHTGRAY);
    }
    else
    {
        DrawText("World Map - ENTER: Enter Local Area | F5: Save | F9: Load", 10, screenHeight - 30, 18, LIGHTGRAY);
        DrawText(TextFormat("World Position: %d,%d | Tile Type: %c", player.x, player.y, worldMap[player.y][player.x].worldTile), 
                10, screenHeight - 55, 18, LIGHTGRAY);
    }
    
    DrawText("WASD/Arrows: Move | R: Reset Camera | Mouse Wheel: Zoom", 10, screenHeight - 80, 18, LIGHTGRAY);
    DrawText("BACKSPACE: Menu/Exit | F: Toggle Fullscreen", 10, screenHeight - 105, 18, LIGHTGRAY);
}