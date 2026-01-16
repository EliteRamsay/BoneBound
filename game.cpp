#include "project.h"

Player player;
GameState currentState = STATE_TITLE;

// Title screen variables
int selectedOption = 0;
bool hasSave = false;

void gamestartup()
{
    InitAudioDevice();
    player = {2, 2};
    currentState = STATE_TITLE;
    selectedOption = 0;
    hasSave = false;
}

void gameupdate()
{
    if (currentState == STATE_TITLE) 
    {
        if(IsKeyPressed(KEY_F))
        {
            togglefullscreen(MAP_WIDTH*TILE_SIZE, MAP_HEIGHT*TILE_SIZE);
        }
        title_update();
    } 
    else if (currentState == STATE_PLAYING) 
    {
        if ((IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) && map[player.y][player.x + 1] != '#') player.x++;
        if ((IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) && map[player.y][player.x - 1] != '#') player.x--;
        if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && map[player.y - 1][player.x] != '#') player.y--;
        if ((IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) && map[player.y + 1][player.x] != '#') player.y++;
        
        if(IsKeyPressed(KEY_F))
        {
            togglefullscreen(MAP_WIDTH*TILE_SIZE, MAP_HEIGHT*TILE_SIZE);
        }

        if (IsKeyPressed(KEY_BACKSPACE)) 
        {
            currentState = STATE_TITLE;
        }
    }
}

void gamedraw()
{
    BeginDrawing();
    ClearBackground(BLACK);
    
    if (currentState == STATE_TITLE) {
        title_draw();
    } else if (currentState == STATE_PLAYING) {
        Vector2 origin = {0, 0};
        draw_map(origin);
        draw_player(origin);
    }
    
    EndDrawing();
}

void gameshutdown()
{
    CloseAudioDevice();
}

void togglefullscreen(int windowWidth, int windowHeight)
{
    if(!IsWindowFullscreen())
    {
        int monitor = GetCurrentMonitor();
        SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
        ToggleFullscreen();
    }
    else
    {
        ToggleFullscreen();
        SetWindowSize(windowWidth, windowHeight);
    }
}

//Main Menu
void title_update()
{
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        selectedOption = (selectedOption + 1) % 2;
    }
    
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        selectedOption = (selectedOption - 1 + 2) % 2;
    }
    
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        if (selectedOption == 0) {
            player = {2, 2};
            currentState = STATE_PLAYING;
        } else if (selectedOption == 1 && hasSave) {
            currentState = STATE_PLAYING;
        }
    }
}

void title_draw()
{
    // Draw game title
    const char* gameTitle = "BoneBound";
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    Vector2 titleSize = MeasureTextEx(GetFontDefault(), gameTitle, 48, 2);
    Vector2 titlePos = {
        (float)screenWidth / 2.0f - titleSize.x / 2.0f,
        (float)screenHeight / 4.0f
    };
    
    DrawTextEx(GetFontDefault(), gameTitle, titlePos, 48, 2, YELLOW);
    
    // Draw menu options
    const char* options[] = {"NEW GAME", "RESUME"};
    int numOptions = 2;
    
    for (int i = 0; i < numOptions; i++) {
        Color color;
        
        if (i == 1 && !hasSave) {
            color = GRAY;
        } else if (i == selectedOption) {
            color = YELLOW;
        } else {
            color = WHITE;
        }
        
        const char* optionText = options[i];
        Vector2 textSize = MeasureTextEx(GetFontDefault(), optionText, 32, 1);
        Vector2 textPos = {
            (float)screenWidth / 2.0f - textSize.x / 2.0f,
            (float)screenHeight / 2.0f + i * 50.0f
        };
        
        // Draw selection arrow for current option
        if (i == selectedOption) {
            DrawText(">", (int)textPos.x - 21, (int)textPos.y, 32, YELLOW);
            DrawText("<", (int)(textPos.x + textSize.x + 10), (int)textPos.y, 32, YELLOW);
        }
        
        DrawTextEx(GetFontDefault(), optionText, textPos, 32, 1, color);
    }
    
    // Draw instructions
    const char* instructions = "Use UP/DOWN to navigate, ENTER to select";
    Vector2 instSize = MeasureTextEx(GetFontDefault(), instructions, 20, 1);
    Vector2 instPos = {
        (float)screenWidth / 2.0f - instSize.x / 2.0f,
        (float)screenHeight - 50.0f
    };
    DrawTextEx(GetFontDefault(), instructions, instPos, 20, 1, LIGHTGRAY);
}

void draw_player(Vector2 origin)
{
    Vector2 pos = 
    {
        origin.x + player.x * TILE_SIZE + 8,
        origin.y + player.y * TILE_SIZE + 6
    };
    
    DrawTextEx(
        GetFontDefault(),
        "D",
        pos,
        24,
        1,
        YELLOW);
}