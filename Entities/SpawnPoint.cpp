#include "SpawnPoint.h"
#include "NPC.h"
#include "../World/LocalMap.h"
#include <time.h>

// ===== VILLAGE SPAWN POINTS (100 NPCs total) =====
void CreateTownSpawnPoints(LocalMap* map)
{
    printf("CreateTownSpawnPoints: Starting for village\n");
    
    // Calculate target spawn points
    int targetCount = 5 + (rand() % 5); // 5-10 spawn points
    printf("Target: %d spawn points\n", targetCount);
    
    // Allocate maximum possible first
    map->spawnPoints = (SpawnPoint**)calloc(targetCount, sizeof(SpawnPoint*));
    if (!map->spawnPoints) {
        printf("ERROR: Failed to allocate spawn points array\n");
        map->spawnPointCount = 0;
        return;
    }
    
    int safeMargin = 30;
    int maxX = map->width - safeMargin;
    int maxY = map->height - safeMargin;
    
    int spawnedCount = 0;
    int attempts = 0;
    int maxAttempts = targetCount * 20; // Prevent infinite loop
    
    while (spawnedCount < targetCount && attempts < maxAttempts) {
        attempts++;
        
        SpawnPoint* spawn = (SpawnPoint*)calloc(1, sizeof(SpawnPoint));
        if (!spawn) {
            printf("ERROR: Failed to allocate spawn point\n");
            continue;
        }
        
        // Find a clear spot
        int x = safeMargin + (rand() % (maxX - safeMargin));
        int y = safeMargin + (rand() % (maxY - safeMargin));
        
        // Check if this spot is on a walkable tile (not a wall)
        bool foundSpot = false;
        if (x >= 0 && x < map->width && y >= 0 && y < map->height) {
            if (map->tiles[y][x] == '.' || map->tiles[y][x] == 'T' || map->tiles[y][x] == ',') {
                foundSpot = true;
            }
        }
        
        if (!foundSpot) {
            free(spawn);
            continue;
        }
        
        // Check if we already have a spawn point too close to this one
        bool tooClose = false;
        for (int i = 0; i < spawnedCount; i++) {
            if (map->spawnPoints[i]) {
                int dx = abs(map->spawnPoints[i]->x - x);
                int dy = abs(map->spawnPoints[i]->y - y);
                if (dx < 10 && dy < 10) { // Minimum distance between spawn points
                    tooClose = true;
                    break;
                }
            }
        }
        
        if (tooClose) {
            free(spawn);
            continue;
        }
        
        // Set spawn properties
        spawn->x = x;
        spawn->y = y;
        spawn->type = SPAWN_HOUSE;
        spawn->npcType = NPC_VILLAGER;
        spawn->isUnique = (rand() % 10 < 2); // 20% unique
        spawn->maxCount = 1 + (rand() % 2); // 1-2 NPCs per spawn
        
        // Generate name
        if (spawn->isUnique) {
            const char* first = FIRST_NAMES[rand() % FIRST_NAME_COUNT];
            const char* last = LAST_NAMES[rand() % LAST_NAME_COUNT];
            snprintf(spawn->npcName, 50, "%s %s", first, last);
        } else {
            snprintf(spawn->npcName, 50, "Villager");
        }
        
        spawn->currentCount = 0;
        spawn->respawnTimer = spawn->isUnique ? 0 : (120 + (rand() % 120));
        spawn->respawnCounter = 0;
        
        map->spawnPoints[spawnedCount] = spawn;
        spawnedCount++;
        
        printf("  Spawn point %d: at (%d,%d), maxCount=%d\n", 
               spawnedCount, x, y, spawn->maxCount);
    }
    
    // Update the actual count
    map->spawnPointCount = spawnedCount;
    printf("CreateTownSpawnPoints: Complete, created %d spawn points (target was %d)\n", 
           spawnedCount, targetCount);
}

// ===== CASTLE SPAWN POINTS (100 NPCs total) =====
void CreateCastleSpawnPoints(LocalMap* map)
{
    map->spawnPointCount = 20; // 20 spawn points = ~100 NPCs
    map->spawnPoints = (SpawnPoint**)malloc(map->spawnPointCount * sizeof(SpawnPoint*));
    
    int safeMargin = 30;
    int maxX = map->width - safeMargin;
    int maxY = map->height - safeMargin;
    
    for (int i = 0; i < map->spawnPointCount; i++) {
        SpawnPoint* spawn = (SpawnPoint*)malloc(sizeof(SpawnPoint));
        
        spawn->x = safeMargin + (rand() % (maxX - safeMargin));
        spawn->y = safeMargin + (rand() % (maxY - safeMargin));
        
        int typeRoll = rand() % 100;
        if (typeRoll < 40) { // 40% Guards (40 NPCs)
            spawn->type = SPAWN_GUARD_POST;
            spawn->npcType = NPC_GUARD;
            spawn->isUnique = false;
            spawn->maxCount = 4 + (rand() % 4); // 4-7 guards per post
            const char* ranks[] = {"Guard", "Knight", "Soldier", "Sentry", "Watchman", "Archer"};
            snprintf(spawn->npcName, 50, ranks[rand() % 6]);
        } else if (typeRoll < 65) { // 25% Servants (25 NPCs)
            spawn->type = SPAWN_HOUSE;
            spawn->npcType = NPC_VILLAGER;
            spawn->isUnique = false;
            spawn->maxCount = 3 + (rand() % 4); // 3-6 servants
            const char* servants[] = {"Servant", "Maid", "Butler", "Cook", "Stablehand", "Page"};
            snprintf(spawn->npcName, 50, servants[rand() % 6]);
        } else if (typeRoll < 80) { // 15% Nobles (15 NPCs)
            spawn->type = SPAWN_HOUSE;
            spawn->npcType = NPC_VILLAGER;
            spawn->isUnique = true;
            spawn->maxCount = 2 + (rand() % 2); // 2-3 nobles per family
            const char* first = FIRST_NAMES[rand() % FIRST_NAME_COUNT];
            const char* last = LAST_NAMES[rand() % LAST_NAME_COUNT];
            const char* titles[] = {"Lord", "Lady", "Baron", "Baroness", "Duke", "Duchess", "Count", "Countess"};
            snprintf(spawn->npcName, 50, "%s %s %s", titles[rand() % 8], first, last);
        } else if (typeRoll < 92) { // 12% Merchants (12 NPCs)
            spawn->type = SPAWN_SHOP;
            spawn->npcType = NPC_MERCHANT;
            spawn->isUnique = true;
            spawn->maxCount = 2 + (rand() % 2); // 2-3 merchants
            const char* first = FIRST_NAMES[rand() % FIRST_NAME_COUNT];
            const char* last = LAST_NAMES[rand() % LAST_NAME_COUNT];
            snprintf(spawn->npcName, 50, "Royal Merchant %s %s", first, last);
        } else { // 8% Blacksmiths (8 NPCs)
            spawn->type = SPAWN_SHOP;
            spawn->npcType = NPC_BLACKSMITH;
            spawn->isUnique = true;
            spawn->maxCount = 2 + (rand() % 2); // 2-3 blacksmiths
            const char* first = FIRST_NAMES[rand() % FIRST_NAME_COUNT];
            const char* last = LAST_NAMES[rand() % LAST_NAME_COUNT];
            snprintf(spawn->npcName, 50, "Royal Blacksmith %s %s", first, last);
        }
        
        spawn->currentCount = 0;
        spawn->respawnTimer = spawn->isUnique ? 0 : (60 + (rand() % 120));
        spawn->respawnCounter = 0;
        
        map->spawnPoints[i] = spawn;
    }
}

// ===== CAVE SPAWN POINTS (100 NPCs total) =====
void CreateCaveSpawnPoints(LocalMap* map)
{
    map->spawnPointCount = 15; // 15 spawn points averaging 6-7 each = ~100
    map->spawnPoints = (SpawnPoint**)malloc(map->spawnPointCount * sizeof(SpawnPoint*));
    
    int safeMargin = 20;
    int maxX = map->width - safeMargin;
    int maxY = map->height - safeMargin;
    
    for (int i = 0; i < map->spawnPointCount; i++) {
        SpawnPoint* spawn = (SpawnPoint*)malloc(sizeof(SpawnPoint));
        
        spawn->x = safeMargin + (rand() % (maxX - safeMargin));
        spawn->y = safeMargin + (rand() % (maxY - safeMargin));
        spawn->type = SPAWN_BANDIT_CAMP;
        spawn->npcType = NPC_BANDIT;
        
        int creatureRoll = rand() % 100;
        if (i == 0 && rand() % 3 == 0) { // Boss creature (unique)
            spawn->isUnique = true;
            const char* first = FIRST_NAMES[rand() % FIRST_NAME_COUNT];
            const char* last = LAST_NAMES[rand() % LAST_NAME_COUNT];
            
            if (creatureRoll < 30) {
                snprintf(spawn->npcName, 50, "Troll %s %s", first, last);
                spawn->maxCount = 1;
            } else if (creatureRoll < 60) {
                snprintf(spawn->npcName, 50, "Ogre %s %s", first, last);
                spawn->maxCount = 1;
            } else {
                snprintf(spawn->npcName, 50, "Cave Monster %s %s", first, last);
                spawn->maxCount = 1;
            }
            spawn->respawnTimer = 0;
        } else {
            spawn->isUnique = false;
            
            if (creatureRoll < 40) {
                snprintf(spawn->npcName, 50, "Goblin");
                spawn->maxCount = 5 + (rand() % 8); // 5-12 goblins
            } else if (creatureRoll < 70) {
                snprintf(spawn->npcName, 50, "Kobold");
                spawn->maxCount = 6 + (rand() % 10); // 6-15 kobolds
            } else {
                snprintf(spawn->npcName, 50, "Bat");
                spawn->maxCount = 8 + (rand() % 12); // 8-19 bats
            }
            spawn->respawnTimer = 120 + (rand() % 180);
        }
        
        spawn->currentCount = 0;
        spawn->respawnCounter = 0;
        
        map->spawnPoints[i] = spawn;
    }
}

// ===== WIZARD TOWER SPAWN POINTS (100 NPCs total) =====
void CreateTowerSpawnPoints(LocalMap* map)
{
    map->spawnPointCount = 12; // 12 spawn points = ~100 NPCs
    map->spawnPoints = (SpawnPoint**)malloc(map->spawnPointCount * sizeof(SpawnPoint*));
    
    int centerX = map->width / 2;
    int centerY = map->height / 2;
    
    for (int i = 0; i < map->spawnPointCount; i++) {
        SpawnPoint* spawn = (SpawnPoint*)malloc(sizeof(SpawnPoint));
        
        spawn->x = centerX - 30 + (rand() % 60);
        spawn->y = centerY - 30 + (rand() % 60);
        spawn->type = SPAWN_HOUSE;
        
        if (i == 0) {
            // The wizard (unique)
            spawn->isUnique = true;
            spawn->npcType = NPC_WIZARD;
            const char* first = FIRST_NAMES[rand() % FIRST_NAME_COUNT];
            const char* last = LAST_NAMES[rand() % LAST_NAME_COUNT];
            
            const char* titles[] = {"Archmage", "Wizard", "Sorcerer", "Magus", "Enchanter", "Warlock"};
            snprintf(spawn->npcName, 50, "%s %s %s", titles[rand() % 6], first, last);
            spawn->maxCount = 1;
            spawn->respawnTimer = 0;
        } else if (i < 5) {
            // Apprentices
            spawn->isUnique = false;
            spawn->npcType = NPC_VILLAGER;
            snprintf(spawn->npcName, 50, "Apprentice");
            spawn->maxCount = 4 + (rand() % 4); // 4-7 apprentices
            spawn->respawnTimer = 180;
        } else {
            // Magical creatures
            spawn->isUnique = false;
            spawn->npcType = NPC_BANDIT;
            const char* creatures[] = {"Golem", "Familiar", "Elemental", "Imp", "Homunculus", "Construct"};
            snprintf(spawn->npcName, 50, creatures[rand() % 6]);
            spawn->maxCount = 3 + (rand() % 5); // 3-7 creatures
            spawn->respawnTimer = 240;
        }
        
        spawn->currentCount = 0;
        spawn->respawnCounter = 0;
        
        map->spawnPoints[i] = spawn;
    }
}

// ===== DUNGEON SPAWN POINTS (100 NPCs total) =====
void CreateDungeonSpawnPoints(LocalMap* map)
{
    map->spawnPointCount = 18; // 18 spawn points = ~100 NPCs
    map->spawnPoints = (SpawnPoint**)malloc(map->spawnPointCount * sizeof(SpawnPoint*));
    
    int safeMargin = 20;
    int maxX = map->width - safeMargin;
    int maxY = map->height - safeMargin;
    
    for (int i = 0; i < map->spawnPointCount; i++) {
        SpawnPoint* spawn = (SpawnPoint*)malloc(sizeof(SpawnPoint));
        
        spawn->x = safeMargin + (rand() % (maxX - safeMargin));
        spawn->y = safeMargin + (rand() % (maxY - safeMargin));
        spawn->type = SPAWN_BANDIT_CAMP;
        
        if (i == 0) { // Boss
            spawn->isUnique = true;
            spawn->npcType = NPC_BANDIT;
            const char* first = FIRST_NAMES[rand() % FIRST_NAME_COUNT];
            const char* last = LAST_NAMES[rand() % LAST_NAME_COUNT];
            const char* titles[] = {"Dungeon Lord", "Torturer", "Prison Master", "Warden", "Overlord"};
            snprintf(spawn->npcName, 50, "%s %s %s", titles[rand() % 5], first, last);
            spawn->maxCount = 1;
            spawn->respawnTimer = 0;
        } else if (i < 5) { // Guards
            spawn->isUnique = false;
            spawn->npcType = NPC_GUARD;
            snprintf(spawn->npcName, 50, "Prison Guard");
            spawn->maxCount = 4 + (rand() % 4); // 4-7 guards
            spawn->respawnTimer = 120;
        } else if (i < 10) { // Undead
            spawn->isUnique = false;
            spawn->npcType = NPC_BANDIT;
            if (rand() % 2 == 0) {
                snprintf(spawn->npcName, 50, "Skeleton");
            } else {
                snprintf(spawn->npcName, 50, "Zombie");
            }
            spawn->maxCount = 4 + (rand() % 6); // 4-9 undead
            spawn->respawnTimer = 180;
        } else { // Prisoners
            spawn->isUnique = false;
            spawn->npcType = NPC_VILLAGER;
            snprintf(spawn->npcName, 50, "Prisoner");
            spawn->maxCount = 3 + (rand() % 5); // 3-7 prisoners
            spawn->respawnTimer = 0; // Prisoners don't respawn
        }
        
        spawn->currentCount = 0;
        spawn->respawnCounter = 0;
        
        map->spawnPoints[i] = spawn;
    }
}

// ===== RUINS SPAWN POINTS (100 NPCs total) =====
void CreateRuinsSpawnPoints(LocalMap* map)
{
    map->spawnPointCount = 15; // 15 spawn points = ~100 NPCs
    map->spawnPoints = (SpawnPoint**)malloc(map->spawnPointCount * sizeof(SpawnPoint*));
    
    int safeMargin = 30;
    int maxX = map->width - safeMargin;
    int maxY = map->height - safeMargin;
    
    for (int i = 0; i < map->spawnPointCount; i++) {
        SpawnPoint* spawn = (SpawnPoint*)malloc(sizeof(SpawnPoint));
        
        spawn->x = safeMargin + (rand() % (maxX - safeMargin));
        spawn->y = safeMargin + (rand() % (maxY - safeMargin));
        spawn->type = SPAWN_BANDIT_CAMP;
        
        int typeRoll = rand() % 100;
        if (typeRoll < 50) { // 50% Grave Robbers (50 NPCs)
            spawn->npcType = NPC_BANDIT;
            snprintf(spawn->npcName, 50, "Grave Robber");
            spawn->isUnique = false;
            spawn->maxCount = 5 + (rand() % 6); // 5-10 robbers
            spawn->respawnTimer = 150;
        } else if (typeRoll < 75) { // 25% Explorers (25 NPCs)
            spawn->npcType = NPC_WANDERER;
            if (rand() % 4 == 0) {
                spawn->isUnique = true;
                const char* first = FIRST_NAMES[rand() % FIRST_NAME_COUNT];
                const char* last = LAST_NAMES[rand() % LAST_NAME_COUNT];
                snprintf(spawn->npcName, 50, "Explorer %s %s", first, last);
                spawn->maxCount = 1;
                spawn->respawnTimer = 0;
            } else {
                spawn->isUnique = false;
                snprintf(spawn->npcName, 50, "Explorer");
                spawn->maxCount = 2 + (rand() % 4); // 2-5 explorers
                spawn->respawnTimer = 200;
            }
        } else { // 25% Hermits (25 NPCs)
            spawn->npcType = NPC_VILLAGER;
            if (rand() % 3 == 0) {
                spawn->isUnique = true;
                const char* first = FIRST_NAMES[rand() % FIRST_NAME_COUNT];
                const char* last = LAST_NAMES[rand() % LAST_NAME_COUNT];
                snprintf(spawn->npcName, 50, "Hermit %s %s", first, last);
                spawn->maxCount = 1;
                spawn->respawnTimer = 0;
            } else {
                spawn->isUnique = false;
                const char* hermits[] = {"Hermit", "Madman", "Recluse", "Loner", "Outcast"};
                snprintf(spawn->npcName, 50, hermits[rand() % 5]);
                spawn->maxCount = 2 + (rand() % 4); // 2-5 hermits
                spawn->respawnTimer = 250;
            }
        }
        
        spawn->currentCount = 0;
        spawn->respawnCounter = 0;
        
        map->spawnPoints[i] = spawn;
    }
}

// ===== FORT SPAWN POINTS (100 NPCs total) =====
void CreateFortSpawnPoints(LocalMap* map)
{
    map->spawnPointCount = 18; // 18 spawn points = ~100 NPCs
    map->spawnPoints = (SpawnPoint**)malloc(map->spawnPointCount * sizeof(SpawnPoint*));
    
    int safeMargin = 30;
    int maxX = map->width - safeMargin;
    int maxY = map->height - safeMargin;
    
    for (int i = 0; i < map->spawnPointCount; i++) {
        SpawnPoint* spawn = (SpawnPoint*)malloc(sizeof(SpawnPoint));
        
        spawn->x = safeMargin + (rand() % (maxX - safeMargin));
        spawn->y = safeMargin + (rand() % (maxY - safeMargin));
        
        int typeRoll = rand() % 100;
        if (typeRoll < 65) { // 65% Soldiers (65 NPCs)
            spawn->type = SPAWN_GUARD_POST;
            spawn->npcType = NPC_GUARD;
            spawn->isUnique = false;
            spawn->maxCount = 5 + (rand() % 6); // 5-10 soldiers
            const char* ranks[] = {"Soldier", "Archer", "Knight", "Swordsman", "Spearman", "Crossbowman"};
            snprintf(spawn->npcName, 50, ranks[rand() % 6]);
        } else if (typeRoll < 85) { // 20% Support Staff (20 NPCs)
            spawn->type = SPAWN_HOUSE;
            spawn->npcType = NPC_VILLAGER;
            spawn->isUnique = false;
            spawn->maxCount = 3 + (rand() % 4); // 3-6 staff
            const char* staff[] = {"Cook", "Blacksmith", "Clerk", "Engineer", "Medic", "Scout"};
            snprintf(spawn->npcName, 50, staff[rand() % 6]);
        } else { // 15% Officers (15 NPCs)
            spawn->type = SPAWN_HOUSE;
            spawn->npcType = NPC_GUARD;
            spawn->isUnique = true;
            spawn->maxCount = 2 + (rand() % 2); // 2-3 officers
            const char* first = FIRST_NAMES[rand() % FIRST_NAME_COUNT];
            const char* last = LAST_NAMES[rand() % LAST_NAME_COUNT];
            const char* officers[] = {"Captain", "Lieutenant", "Sergeant", "Commander", "General"};
            snprintf(spawn->npcName, 50, "%s %s %s", officers[rand() % 5], first, last);
        }
        
        spawn->currentCount = 0;
        spawn->respawnTimer = spawn->isUnique ? 0 : (90 + (rand() % 120));
        spawn->respawnCounter = 0;
        
        map->spawnPoints[i] = spawn;
    }
}

// ===== BANDIT CAMP SPAWN POINTS (10 NPCs total - non-location) =====
void CreateBanditCampSpawnPoints(LocalMap* map)
{
    map->spawnPointCount = 3; // 3 spawn points = ~10 NPCs
    map->spawnPoints = (SpawnPoint**)malloc(map->spawnPointCount * sizeof(SpawnPoint*));
    
    int safeMargin = 30;
    int maxX = map->width - safeMargin;
    int maxY = map->height - safeMargin;
    
    for (int i = 0; i < map->spawnPointCount; i++) {
        SpawnPoint* spawn = (SpawnPoint*)malloc(sizeof(SpawnPoint));
        
        spawn->x = safeMargin + (rand() % (maxX - safeMargin));
        spawn->y = safeMargin + (rand() % (maxY - safeMargin));
        spawn->type = SPAWN_BANDIT_CAMP;
        spawn->npcType = NPC_BANDIT;
        
        if (i == 0 && rand() % 2 == 0) {
            spawn->isUnique = true;
            const char* first = FIRST_NAMES[rand() % FIRST_NAME_COUNT];
            const char* last = LAST_NAMES[rand() % LAST_NAME_COUNT];
            snprintf(spawn->npcName, 50, "Bandit Leader %s %s", first, last);
            spawn->maxCount = 1;
            spawn->respawnTimer = 0;
        } else {
            spawn->isUnique = false;
            snprintf(spawn->npcName, 50, "Bandit");
            spawn->maxCount = 3 + (rand() % 4); // 3-6 bandits
            spawn->respawnTimer = 120 + (rand() % 120);
        }
        
        spawn->currentCount = 0;
        spawn->respawnCounter = 0;
        
        map->spawnPoints[i] = spawn;
    }
}

// ===== SPAWN NPCS FROM POINTS =====
void SpawnNPCsFromPoints(LocalMap* map, int worldX, int worldY)
{
    if (!map || !map->spawnPoints) return;
    
    // First, count how many NPCs already exist in this map
    int existingCount = 0;
    for (int i = 0; i < npcCount; i++) {
        if (npcs[i] && npcs[i]->worldX == worldX && npcs[i]->worldY == worldY) {
            existingCount++;
        }
    }
    
    // If we already have NPCs, don't spawn more
    if (existingCount > 0) {
        printf("Map (%d,%d) already has %d NPCs, skipping spawn\n", worldX, worldY, existingCount);
        return;
    }
    
    // Calculate total potential NPCs from all spawn points
    int totalPotential = 0;
    for (int i = 0; i < map->spawnPointCount; i++) {
        totalPotential += map->spawnPoints[i]->maxCount;
    }
    
    // Cap at MAX_NPCS_PER_MAP
    if (totalPotential > MAX_NPCS_PER_MAP) {
        printf("Warning: Map (%d,%d) would have %d NPCs, capping to %d\n", 
               worldX, worldY, totalPotential, MAX_NPCS_PER_MAP);
        
        // Scale down all spawn points proportionally
        float scale = (float)MAX_NPCS_PER_MAP / totalPotential;
        for (int i = 0; i < map->spawnPointCount; i++) {
            map->spawnPoints[i]->maxCount = 
                (int)(map->spawnPoints[i]->maxCount * scale);
            if (map->spawnPoints[i]->maxCount < 1) 
                map->spawnPoints[i]->maxCount = 1;
        }
    }
    
    // Now spawn NPCs
    for (int i = 0; i < map->spawnPointCount; i++) {
        SpawnPoint* spawn = map->spawnPoints[i];
        
        // Check global limit
        if (npcCount + spawn->maxCount > MAX_TOTAL_NPCS) {
            printf("Global NPC limit reached! Cannot spawn more.\n");
            return;
        }
        
        for (int j = 0; j < spawn->maxCount; j++) {
            // Check if this specific NPC slot is already filled
            bool alreadyExists = false;
            for (int k = 0; k < npcCount; k++) {
                if (npcs[k] && npcs[k]->spawnPoint == spawn && 
                    npcs[k]->localX == spawn->x && npcs[k]->localY == spawn->y) {
                    alreadyExists = true;
                    break;
                }
            }
            
            if (!alreadyExists) {
                NPC* npc = CreateNPCFromSpawn(spawn, worldX, worldY);
                if (npc) {
                    npcs = (NPC**)realloc(npcs, (npcCount + 1) * sizeof(NPC*));
                    npcs[npcCount] = npc;
                    npcCount++;
                    spawn->currentCount++;
                }
            }
        }
    }
    
    printf("Map (%d,%d) now has %d NPCs\n", worldX, worldY, npcCount - existingCount);
}

// ===== UPDATE SPAWN POINTS (RESPAWN GENERIC NPCS) =====
void UpdateSpawnPoints()
{
    if (!player.isInLocalMap || !worldMap) return;
    
    LocalMap* map = worldMap[player.worldPos.y][player.worldPos.x].localMap;
    if (!map || !map->spawnPoints) return;
    
    for (int i = 0; i < map->spawnPointCount; i++) {
        SpawnPoint* spawn = map->spawnPoints[i];
        
        // Only respawn generic NPCs
        if (!spawn->isUnique && spawn->currentCount < spawn->maxCount && spawn->respawnTimer > 0) {
            spawn->respawnCounter += GetFrameTime();
            
            if (spawn->respawnCounter >= spawn->respawnTimer) {
                int needed = spawn->maxCount - spawn->currentCount;
                for (int j = 0; j < needed; j++) {
                    NPC* npc = CreateNPCFromSpawn(spawn, player.worldPos.x, player.worldPos.y);
                    
                    npcs = (NPC**)realloc(npcs, (npcCount + 1) * sizeof(NPC*));
                    npcs[npcCount] = npc;
                    npcCount++;
                    spawn->currentCount++;
                }
                spawn->respawnCounter = 0;
            }
        }
    }
}

// ===== NPC DIED =====
void NPCDied(NPC* npc)
{
    if (!npc->spawnPoint) return;
    
    npc->spawnPoint->currentCount--;
    
    if (npc->isUnique) {
        // Unique NPCs are gone forever
        npc->spawnPoint->maxCount = 0;
    }
}