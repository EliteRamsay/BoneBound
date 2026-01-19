#include "project.h"

// Global game variables
Player player;
GameState currentState = STATE_TITLE;
GameCamera gameCamera;

// Menu variables
int selectedOption = 0;
bool hasSave = false;

// Map data
char** map = NULL;
int currentMapWidth = MAP_WIDTH;
int currentMapHeight = MAP_HEIGHT;

// Map sizes with default zoom levels
MapConfig mapSizes[NUM_SIZES] = {
    {8, 8, "TINY", 4.0f},
    {16, 16, "SMALL", 2.0f},
    {32, 32, "MEDIUM", 1.5f},
    {64, 64, "LARGE", 0.8f},
    {128, 128, "HUGE", 0.4f},
    {256, 256, "GIGANTIC", 0.2f}
};

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
    Vector2 targetPos = { 
        (float)(player.x * TILE_SIZE + TILE_SIZE / 2.0f), 
        (float)(player.y * TILE_SIZE + TILE_SIZE / 2.0f) 
    };
    
    // Smooth camera movement
    float lerpSpeed = 0.15f;
    gameCamera.camera.target.x += (targetPos.x - gameCamera.camera.target.x) * lerpSpeed;
    gameCamera.camera.target.y += (targetPos.y - gameCamera.camera.target.y) * lerpSpeed;
    
    float mapWidthWorld = currentMapWidth * TILE_SIZE;
    float mapHeightWorld = currentMapHeight * TILE_SIZE;
    
    // Handle zoom with mouse wheel
    float wheelMove = GetMouseWheelMove();
    if (wheelMove != 0) {
        float zoomChange = wheelMove * 0.1f * gameCamera.zoom;
        float newZoom = gameCamera.zoom + zoomChange;
        
        // Set zoom limits based on map size
        float minZoom, maxZoom;
        
        if (currentMapWidth <= 8 && currentMapHeight <= 8) {
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

// Convert world to screen coordinates
Vector2 get_world_to_screen(Vector2 worldPos)
{
    return GetWorldToScreen2D(worldPos, gameCamera.camera);
}

// Initialize game
void gamestartup()
{
    InitAudioDevice();
    player = {2, 2};
    currentState = STATE_TITLE;
    selectedOption = 0;
    hasSave = false;
}

// Update game logic
void gameupdate()
{
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
        mapsize_update();
    }
    else if (currentState == STATE_PLAYING) 
    {
        // Store old position
        int oldX = player.x;
        int oldY = player.y;
        
        // Player movement with collision
        if ((IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) && 
            player.x + 1 < currentMapWidth && 
            map[player.y][player.x + 1] != '#') player.x++;
        
        if ((IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) && 
            player.x > 0 && 
            map[player.y][player.x - 1] != '#') player.x--;
        
        if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && 
            player.y > 0 && 
            map[player.y - 1][player.x] != '#') player.y--;
        
        if ((IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) && 
            player.y + 1 < currentMapHeight && 
            map[player.y + 1][player.x] != '#') player.y++;
        
        // Update camera if player moved
        if (oldX != player.x || oldY != player.y)
        {
            update_camera();
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

        // Return to menu
        if (IsKeyPressed(KEY_BACKSPACE)) 
        {
            currentState = STATE_TITLE;
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
    else if (currentState == STATE_PLAYING) 
    {
        BeginMode2D(gameCamera.camera);
        
        // Draw map and player
        draw_map();
        draw_player();
        
        EndMode2D();
        
        // Draw HUD
        draw_hud();
    }
    
    EndDrawing();
}

// Clean up game
void gameshutdown()
{
    cleanup_map();
    CloseAudioDevice();
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

// Free map memory
void cleanup_map()
{
    if (map != NULL)
    {
        for (int i = 0; i < currentMapHeight; i++)
        {
            free(map[i]);
        }
        free(map);
        map = NULL;
    }
}

// Create random map
void generate_random_map(int width, int height)
{
    cleanup_map();
    
    currentMapWidth = width;
    currentMapHeight = height;
    
    // Allocate map memory
    map = (char**)malloc(height * sizeof(char*));
    for (int y = 0; y < height; y++)
    {
        map[y] = (char*)malloc((width + 1) * sizeof(char));
        
        for (int x = 0; x < width; x++)
        {
            // Border walls
            if (x == 0 || x == width - 1 || y == 0 || y == height - 1)
            {
                map[y][x] = '#';
            }
            else
            {
                // Random terrain
                float randVal = (float)rand() / RAND_MAX;
                
                if (randVal < 0.05)
                    map[y][x] = '~';
                else if (randVal < 0.15)
                    map[y][x] = '^';
                else if (randVal < 0.20)
                    map[y][x] = 'T';
                else
                    map[y][x] = '.';
            }
        }
        map[y][width] = '\0';
    }
    
    // Clear starting area
    for (int y = 1; y <= 3; y++)
    {
        for (int x = 1; x <= 3; x++)
        {
            if (y < height && x < width)
                map[y][x] = '.';
        }
    }
    
    // Set player start
    player.x = 2;
    player.y = 2;
    
    // Setup camera
    init_camera();
}

// Update title screen
void title_update()
{
    // Menu navigation
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        selectedOption = (selectedOption + 1) % 2;
    }
    
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        selectedOption = (selectedOption - 1 + 2) % 2;
    }
    
    // Menu selection
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        if (selectedOption == 0) {
            selectedOption = 0;
            currentState = STATE_MAPSIZE;
        } else if (selectedOption == 1 && hasSave) {
            currentState = STATE_PLAYING;
            init_camera();
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
    const char* options[] = {"NEW GAME", "RESUME"};
    
    for (int i = 0; i < 2; i++) {
        Color color = (i == selectedOption) ? YELLOW : WHITE;
        if (i == 1 && !hasSave) color = GRAY;
        
        const char* optionText = options[i];
        Vector2 textSize = MeasureTextEx(GetFontDefault(), optionText, 32, 1);
        Vector2 textPos = {
            (float)screenWidth / 2.0f - textSize.x / 2.0f,
            (float)screenHeight / 2.0f + i * 50.0f
        };
        
        // Selection arrows
        if (i == selectedOption) {
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
        generate_random_map(mapSizes[selectedOption].width, mapSizes[selectedOption].height);
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

// Draw player
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

// Draw HUD
void draw_hud()
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Player position
    char posText[50];
    sprintf(posText, "Position: (%d, %d)", player.x, player.y);
    DrawText(posText, 10, 10, 20, WHITE);
    
    // Map info
    char mapText[100];
    sprintf(mapText, "Map: %s (%dx%d) Zoom: %.1fx", 
        mapSizes[selectedOption].name, 
        currentMapWidth, currentMapHeight, gameCamera.zoom);
    DrawText(mapText, 10, 35, 20, WHITE);
    
    // Camera position
    char camText[100];
    sprintf(camText, "Camera: (%.0f, %.0f)", 
        gameCamera.camera.target.x, gameCamera.camera.target.y);
    DrawText(camText, 10, 60, 20, LIGHTGRAY);
    
    // Instructions
    DrawText("WASD/Arrows: Move | R: Reset Camera | Mouse Wheel: Zoom", 10, screenHeight - 30, 18, LIGHTGRAY);
    DrawText("BACKSPACE: Menu | F: Toggle Fullscreen", 10, screenHeight - 55, 18, LIGHTGRAY);
}