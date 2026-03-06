#include "Menus.h"
#include "../World/WorldMap.h"
#include "../Systems/SaveLoad.h"

void TitleUpdate()
{
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
        selectedOption = (selectedOption + 1) % 3;
    
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
        selectedOption = (selectedOption - 1 + 3) % 3;
    
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        if (selectedOption == 0) {
            currentState = STATE_MAPSIZE;
        } else if (selectedOption == 1) {
            if (SaveFileExists(0) || SaveFileExists(1) || SaveFileExists(2))
                currentState = STATE_LOAD_MENU;
        } else if (selectedOption == 2) {
            shouldQuit = true;
        }
    }
}

void TitleDraw()
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Version in bottom left - fixed narrowing conversion
    const char* version = "v0.0.6";
    DrawTextEx(GetFontDefault(), version, (Vector2){ 10.0f, (float)(screenHeight - 25) }, 16, 1, DARKGRAY);
    
    // Title
    const char* title = "BoneBound";
    Vector2 titleSize = MeasureTextEx(GetFontDefault(), title, 48, 2);
    Vector2 titlePos = {
        screenWidth / 2.0f - titleSize.x / 2.0f,
        screenHeight / 4.0f
    };
    DrawTextEx(GetFontDefault(), title, titlePos, 48, 2, YELLOW);
    
    // Options
    const char* options[] = {"NEW GAME", "LOAD GAME", "QUIT"};
    bool hasSaves = SaveFileExists(0) || SaveFileExists(1) || SaveFileExists(2);
    
    for (int i = 0; i < 3; i++) {
        Color color = (i == selectedOption) ? YELLOW : WHITE;
        
        if (i == 1 && !hasSaves)
            color = GRAY;
        
        Vector2 textSize = MeasureTextEx(GetFontDefault(), options[i], 32, 1);
        Vector2 textPos = {
            screenWidth / 2.0f - textSize.x / 2.0f,
            screenHeight / 2.0f + i * 50.0f
        };
        
        bool isGrayedOut = (i == 1 && !hasSaves);
        if (i == selectedOption && !isGrayedOut) {
            DrawText(">", textPos.x - 21, textPos.y, 32, YELLOW);
            DrawText("<", textPos.x + textSize.x + 10, textPos.y, 32, YELLOW);
        }
        
        DrawTextEx(GetFontDefault(), options[i], textPos, 32, 1, color);
    }
    
    // Instructions
    const char* inst = "UP/DOWN to navigate, ENTER to select";
    Vector2 instSize = MeasureTextEx(GetFontDefault(), inst, 20, 1);
    Vector2 instPos = {
        screenWidth / 2.0f - instSize.x / 2.0f,
        (float)screenHeight - 50.0f
    };
    DrawTextEx(GetFontDefault(), inst, instPos, 20, 1, LIGHTGRAY);
}

void MapSizeUpdate()
{
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
        selectedOption = (selectedOption + 1) % NUM_SIZES;
    
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
        selectedOption = (selectedOption - 1 + NUM_SIZES) % NUM_SIZES;
    
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        GenerateWorldMap(mapSizes[selectedOption].width, mapSizes[selectedOption].height);
        currentState = STATE_PLAYING;
    }
    
    if (IsKeyPressed(KEY_BACKSPACE))
        currentState = STATE_TITLE;
}

void MapSizeDraw()
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Version in bottom left - fixed narrowing conversion
    const char* version = "v0.0.6";
    DrawTextEx(GetFontDefault(), version, (Vector2){ 10.0f, (float)(screenHeight - 25) }, 16, 1, DARKGRAY);
    
    // Title
    const char* title = "Map Size";
    Vector2 titleSize = MeasureTextEx(GetFontDefault(), title, 48, 2);
    Vector2 titlePos = {
        screenWidth / 2.0f - titleSize.x / 2.0f,
        screenHeight / 6.0f
    };
    DrawTextEx(GetFontDefault(), title, titlePos, 48, 2, YELLOW);
    
    // Selected info
    const char* info = TextFormat("Selected: %s (%dx%d)", 
        mapSizes[selectedOption].name,
        mapSizes[selectedOption].width,
        mapSizes[selectedOption].height);
    Vector2 infoSize = MeasureTextEx(GetFontDefault(), info, 22, 1);
    Vector2 infoPos = {
        screenWidth / 2.0f - infoSize.x / 2.0f,
        screenHeight / 3.0f
    };
    DrawTextEx(GetFontDefault(), info, infoPos, 22, 1, LIGHTGRAY);
    
    // Options
    const char* names[] = {"TINY", "SMALL", "MEDIUM", "LARGE", "HUGE", "GIGANTIC"};
    
    for (int i = 0; i < NUM_SIZES; i++) {
        Color color = (i == selectedOption) ? YELLOW : WHITE;
        
        char text[50];
        sprintf(text, "%s (%dx%d)", names[i], mapSizes[i].width, mapSizes[i].height);
        
        Vector2 textSize = MeasureTextEx(GetFontDefault(), text, 28, 1);
        Vector2 textPos = {
            screenWidth / 2.0f - textSize.x / 2.0f,
            screenHeight / 2.0f + i * 40.0f
        };
        
        if (i == selectedOption) {
            DrawText(">", textPos.x - 21, textPos.y, 28, YELLOW);
            DrawText("<", textPos.x + textSize.x + 10, textPos.y, 28, YELLOW);
        }
        
        DrawTextEx(GetFontDefault(), text, textPos, 28, 1, color);
    }
}

void CharacterCreationDraw()
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Version in bottom left - fixed narrowing conversion
    const char* version = "v0.0.6";
    DrawTextEx(GetFontDefault(), version, (Vector2){ 10.0f, (float)(screenHeight - 25) }, 16, 1, DARKGRAY);
    
    // Title
    const char* title = "Character Creation";
    Vector2 titleSize = MeasureTextEx(GetFontDefault(), title, 48, 2);
    Vector2 titlePos = {
        screenWidth / 2.0f - titleSize.x / 2.0f,
        screenHeight / 6.0f
    };
    DrawTextEx(GetFontDefault(), title, titlePos, 48, 2, YELLOW);
    
    // Placeholder text
    const char* placeholder = "Select your race!";
    Vector2 phSize = MeasureTextEx(GetFontDefault(), placeholder, 28, 1);
    Vector2 phPos = {
        screenWidth / 2.0f - phSize.x / 2.0f,
        screenHeight / 2.0f
    };
    DrawTextEx(GetFontDefault(), placeholder, phPos, 28, 1, LIGHTGRAY);

    //Pick between dog and cat, with cat being default.
    const char* options[] = {"Dog", "Cat"};
    for (int i = 0; i < 2; i++) {
        Color color = (i == 1) ? YELLOW : WHITE;
        
        Vector2 textSize = MeasureTextEx(GetFontDefault(), options[i], 28, 1);
        Vector2 textPos = {
            screenWidth / 2.0f - textSize.x / 2.0f,
            screenHeight / 2.0f + 50.0f + i * 40.0f
        };
        
        if (i == 1) {
            DrawText(">", textPos.x - 21, textPos.y, 28, YELLOW);
            DrawText("<", textPos.x + textSize.x + 10, textPos.y, 28, YELLOW);
        }
        
        DrawTextEx(GetFontDefault(), options[i], textPos, 28, 1, color);
    }
}

void CharacterCreationUpdate()
{
    // Placeholder for character creation logic. For now, just toggle between dog and cat with up/down and confirm with enter.
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
        selectedOption = (selectedOption + 1) % 2;
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
        selectedOption = (selectedOption - 1 + 2) % 2;
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        // Set players isCat based on selection, then move player to playing state
        player.isCat = (selectedOption == 1);
        currentState = STATE_PLAYING;
    }
}



void SaveMenuUpdate()
{
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
        saveSlotSelected = (saveSlotSelected + 1) % 3;
    
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
        saveSlotSelected = (saveSlotSelected - 1 + 3) % 3;
    
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        SaveGame(saveSlotSelected);
        currentState = STATE_PLAYING;
    }
    
    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_ESCAPE)) {
        // Make sure we're returning to a valid state
        if (worldMap != NULL) {
            currentState = STATE_PLAYING;
        } else {
            // If no world map exists, go back to title
            currentState = STATE_TITLE;
            selectedOption = 0;
        }
    }
}

void SaveMenuDraw()
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Version in bottom left - fixed narrowing conversion
    const char* version = "v0.0.6";
    DrawTextEx(GetFontDefault(), version, (Vector2){ 10.0f, (float)(screenHeight - 25) }, 16, 1, DARKGRAY);
    
    // Title
    const char* title = "Save Game";
    Vector2 titleSize = MeasureTextEx(GetFontDefault(), title, 48, 2);
    Vector2 titlePos = {
        screenWidth / 2.0f - titleSize.x / 2.0f,
        screenHeight / 6.0f
    };
    DrawTextEx(GetFontDefault(), title, titlePos, 48, 2, YELLOW);
    
    // Slots
    const char* slots[] = {"Save Slot 1", "Save Slot 2", "Save Slot 3"};
    
    for (int i = 0; i < 3; i++) {
        Color color = (i == saveSlotSelected) ? YELLOW : WHITE;
        
        Vector2 textSize = MeasureTextEx(GetFontDefault(), slots[i], 32, 1);
        Vector2 textPos = {
            screenWidth / 2.0f - textSize.x / 2.0f,
            screenHeight / 2.0f + i * 50.0f
        };
        
        if (i == saveSlotSelected) {
            DrawText(">", textPos.x - 21, textPos.y, 32, YELLOW);
            DrawText("<", textPos.x + textSize.x + 10, textPos.y, 32, YELLOW);
        }
        
        DrawTextEx(GetFontDefault(), slots[i], textPos, 32, 1, color);
    }
}

void LoadMenuUpdate()
{
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
        saveSlotSelected = (saveSlotSelected + 1) % 3;
    
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
        saveSlotSelected = (saveSlotSelected - 1 + 3) % 3;
    
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        if (SaveFileExists(saveSlotSelected) && LoadGame(saveSlotSelected)) {
            currentState = STATE_PLAYING;
        }
    }
    
    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_ESCAPE)) {
        // Make sure we're returning to a valid state
        if (worldMap != NULL) {
            currentState = STATE_PLAYING;
        } else {
            // If no world map exists, go back to title
            currentState = STATE_TITLE;
            selectedOption = 0;
        }
    }
}

void LoadMenuDraw()
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Version in bottom left - fixed narrowing conversion
    const char* version = "v0.0.6";
    DrawTextEx(GetFontDefault(), version, (Vector2){ 10.0f, (float)(screenHeight - 25) }, 16, 1, DARKGRAY);
    
    // Title
    const char* title = "Load Game";
    Vector2 titleSize = MeasureTextEx(GetFontDefault(), title, 48, 2);
    Vector2 titlePos = {
        screenWidth / 2.0f - titleSize.x / 2.0f,
        screenHeight / 6.0f
    };
    DrawTextEx(GetFontDefault(), title, titlePos, 48, 2, YELLOW);
    
    // Slots
    const char* slots[] = {"Load Slot 1", "Load Slot 2", "Load Slot 3"};
    
    for (int i = 0; i < 3; i++) {
        bool exists = SaveFileExists(i);
        Color color = (i == saveSlotSelected && exists) ? YELLOW : (exists ? WHITE : GRAY);
        
        Vector2 textSize = MeasureTextEx(GetFontDefault(), slots[i], 32, 1);
        Vector2 textPos = {
            screenWidth / 2.0f - textSize.x / 2.0f,
            screenHeight / 2.0f + i * 50.0f
        };
        
        if (i == saveSlotSelected && exists) {
            DrawText(">", textPos.x - 21, textPos.y, 32, YELLOW);
            DrawText("<", textPos.x + textSize.x + 10, textPos.y, 32, YELLOW);
        }
        
        DrawTextEx(GetFontDefault(), slots[i], textPos, 32, 1, color);
        
        if (!exists) {
            Vector2 emptySize = MeasureTextEx(GetFontDefault(), "(Empty)", 20, 1);
            Vector2 emptyPos = {
                screenWidth / 2.0f - emptySize.x / 2.0f,
                textPos.y + 35.0f
            };
            DrawTextEx(GetFontDefault(), "(Empty)", emptyPos, 20, 1, GRAY);
        }
    }
}