#include "NPC.h"
#include "SpawnPoint.h"
#include "../World/LocalMap.h"  // Make sure this is here
#include "../Game/Camera.h"
#include <time.h>

// ===== NAME DATABASE DEFINITIONS =====
const char* FIRST_NAMES[] = {
    // Male
    "John", "William", "James", "Robert", "Michael", "Thomas", "Richard", "Charles",
    "Henry", "Edward", "George", "Andrew", "David", "Daniel", "Matthew", "Christopher",
    "Anthony", "Donald", "Mark", "Paul", "Steven", "Kenneth", "Joshua", "Kevin",
    "Brian", "Timothy", "Ronald", "Jason", "Jeffrey", "Ryan", "Jacob", "Gary",
    "Nicholas", "Eric", "Stephen", "Jonathan", "Larry", "Justin", "Scott", "Brandon",
    "Benjamin", "Samuel", "Gregory", "Frank", "Alexander", "Raymond", "Patrick", "Jack",
    "Dennis", "Jerry", "Tyler", "Aaron", "Jose", "Adam", "Nathan", "Peter", "Zachary",
    "Walter", "Harold", "Douglas", "Carl", "Arthur", "Roger", "Keith", "Gerald",
    "Lawrence", "Terry", "Sean", "Austin", "Christian", "Jesse", "Ethan", "Billy",
    "Bruce", "Ralph", "Roy", "Jordan", "Bryan", "Alan", "Wayne", "Eugene", "Logan",
    "Randy", "Louis", "Russell", "Vincent", "Philip", "Bobby", "Johnny", "Bradley",
    
    // Female
    "Mary", "Patricia", "Jennifer", "Linda", "Elizabeth", "Barbara", "Susan", "Jessica",
    "Sarah", "Karen", "Nancy", "Lisa", "Betty", "Margaret", "Sandra", "Ashley",
    "Kimberly", "Emily", "Donna", "Michelle", "Dorothy", "Carol", "Amanda", "Melissa",
    "Deborah", "Stephanie", "Rebecca", "Sharon", "Laura", "Cynthia", "Kathleen", "Amy",
    "Shirley", "Angela", "Helen", "Anna", "Brenda", "Pamela", "Nicole", "Samantha",
    "Katherine", "Emma", "Ruth", "Christine", "Catherine", "Debra", "Rachel", "Carolyn",
    "Janet", "Virginia", "Maria", "Heather", "Diane", "Julie", "Joyce", "Victoria",
    "Kelly", "Christina", "Lauren", "Joan", "Evelyn", "Olivia", "Judith", "Megan",
    "Cheryl", "Martha", "Andrea", "Frances", "Hannah", "Jacqueline", "Ann", "Gloria",
    "Jean", "Kathryn", "Alice", "Teresa", "Sara", "Janice", "Doris", "Madison",
    "Julia", "Grace", "Judy", "Abigail", "Marie", "Denise", "Beverly", "Amber",
    
    // Fantasy/Medieval
    "Aric", "Brom", "Cedric", "Dorian", "Eldric", "Finnian", "Gareth", "Hadrian",
    "Ivor", "Jorah", "Kaelen", "Leofric", "Merek", "Nolan", "Orin", "Percival",
    "Quinn", "Roland", "Sylas", "Theron", "Urien", "Valen", "Warrick", "Xander",
    "Yorick", "Zane", "Aelwen", "Briar", "Caelia", "Dwyn", "Elara", "Faelan",
    "Gwyneth", "Hestia", "Isolde", "Juniper", "Kaelia", "Lirien", "Morwen", "Nimue",
    "Oriana", "Phaedra", "Quenlya", "Rhiannon", "Seraphina", "Talia", "Una", "Vivienne",
    "Wynne", "Xanthe", "Ysmay", "Zephyra"
};

const char* LAST_NAMES[] = {
    // Common
    "Smith", "Johnson", "Williams", "Brown", "Jones", "Garcia", "Miller", "Davis",
    "Rodriguez", "Martinez", "Hernandez", "Lopez", "Gonzalez", "Wilson", "Anderson",
    "Thomas", "Taylor", "Moore", "Jackson", "Martin", "Lee", "Perez", "Thompson",
    "White", "Harris", "Sanchez", "Clark", "Ramirez", "Lewis", "Robinson", "Walker",
    "Young", "Allen", "King", "Wright", "Scott", "Torres", "Nguyen", "Hill", "Flores",
    "Green", "Adams", "Nelson", "Baker", "Hall", "Rivera", "Campbell", "Mitchell",
    "Carter", "Roberts", "Gomez", "Phillips", "Evans", "Turner", "Diaz", "Parker",
    "Cruz", "Edwards", "Collins", "Reyes", "Stewart", "Morris", "Morales", "Murphy",
    "Cook", "Rogers", "Gutierrez", "Ortiz", "Morgan", "Cooper", "Peterson", "Bailey",
    "Reed", "Kelly", "Howard", "Ramos", "Kim", "Cox", "Ward", "Richardson", "Watson",
    "Brooks", "Chavez", "Wood", "James", "Bennett", "Gray", "Mendoza", "Ruiz",
    "Hughes", "Price", "Alvarez", "Castillo", "Sanders", "Patel", "Myers", "Long",
    
    // Medieval/Occupation
    "Baker", "Butcher", "Carpenter", "Cooper", "Fletcher", "Gardner", "Hunter",
    "Mason", "Miller", "Miner", "Potter", "Sawyer", "Shepherd", "Smith", "Taylor",
    "Thatcher", "Weaver", "Wright", "Archer", "Bowman", "Carter", "Chandler",
    "Clark", "Cook", "Fisher", "Forester", "Fuller", "Glover", "Harper", "Hawkins",
    "Hayward", "Head", "Herd", "Hunt", "Kemp", "Knight", "Marshall", "Page",
    "Parker", "Parson", "Porter", "Reeve", "Saddler", "Sargeant", "Sexton",
    "Sherman", "Spencer", "Steward", "Stoddard", "Summers", "Ward", "Webb",
    
    // Location-based
    "Hill", "Lake", "Wood", "Forest", "Field", "Marsh", "Moore", "Brooks",
    "Rivers", "Stone", "Bridge", "Ford", "Haven", "Shore", "Creek", "Valley",
    "Meadows", "Grove", "Thorne", "Wells", "Springs", "Underwood", "Atwood",
    "Blackwood", "Greenwood", "Holbrook", "Linwood", "Oakley", "Prescott",
    "Radcliff", "Redmond", "Sherwood", "Stanley", "Sutton", "Thornton",
    "Wesley", "Weston", "Whitman", "Woodward", "Worth",
    
    // Fantasy
    "Ironheart", "Stormborn", "Winterfell", "Blackthorn", "Silverwood", "Goldmane",
    "Oakenshield", "Moonshadow", "Dragonborn", "Starweaver", "Frostbeard",
    "Lightfoot", "Darkwater", "Swiftarrow", "Strongbow", "Fairwind", "Ravenclaw",
    "Griffin", "Phoenix", "Stormcrow", "Whitewolf", "Redhand", "Graywolf",
    "Blackraven", "Silverstream", "Goldenleaf", "Copperfield", "Ironwood",
    "Steelheart", "Bronzebeard", "Silverblade", "Goldfinder", "Stonehelm",
    "Ironfist", "Strongarm", "Swiftwind", "Farsight", "Dawnbringer"
};

const int FIRST_NAME_COUNT = sizeof(FIRST_NAMES) / sizeof(FIRST_NAMES[0]);
const int LAST_NAME_COUNT = sizeof(LAST_NAMES) / sizeof(LAST_NAMES[0]);

// Generate a random name for NPCs
void GenerateNPCName(char* buffer, int bufferSize, bool isUnique)
{
    if (isUnique) {
        // Unique NPCs get a first and last name
        const char* firstName = FIRST_NAMES[rand() % FIRST_NAME_COUNT];
        const char* lastName = LAST_NAMES[rand() % LAST_NAME_COUNT];
        snprintf(buffer, bufferSize, "%s %s", firstName, lastName);
    } else {
        // Generic NPCs just get a title + first name
        const char* titles[] = {"Guard", "Traveler", "Bandit", "Villager", "Merchant", "Peasant"};
        const char* firstName = FIRST_NAMES[rand() % FIRST_NAME_COUNT];
        
        int titleIndex = rand() % 6;
        if (titleIndex == 0) snprintf(buffer, bufferSize, "Guard %s", firstName);
        else if (titleIndex == 1) snprintf(buffer, bufferSize, "Traveler %s", firstName);
        else if (titleIndex == 2) snprintf(buffer, bufferSize, "Bandit %s", firstName);
        else if (titleIndex == 3) snprintf(buffer, bufferSize, "%s the Villager", firstName);
        else if (titleIndex == 4) snprintf(buffer, bufferSize, "Merchant %s", firstName);
        else snprintf(buffer, bufferSize, "Peasant %s", firstName);
    }
}

// Create NPC from spawn point with random positioning
NPC* CreateNPCFromSpawn(SpawnPoint* spawn, int worldX, int worldY)
{
    if (!spawn) {
        printf("ERROR: spawn is NULL in CreateNPCFromSpawn\n");
        return NULL;
    }
    
    NPC* npc = (NPC*)calloc(1, sizeof(NPC));
    if (!npc) return NULL;
    
    npc->id = npcCount;
    npc->worldX = worldX;
    npc->worldY = worldY;
    npc->isInLocalMap = true;
    npc->isActive = true;
    npc->spawnPoint = spawn;
    npc->isUnique = spawn->isUnique;
    
    // Safe position within map
    npc->localX = spawn->x;
    npc->localY = spawn->y;
    
    // Clamp to safe bounds
    if (npc->localX < 1) npc->localX = 1;
    if (npc->localY < 1) npc->localY = 1;
    if (npc->localX > LOCAL_MAP_WIDTH - 2) npc->localX = LOCAL_MAP_WIDTH - 2;
    if (npc->localY > LOCAL_MAP_HEIGHT - 2) npc->localY = LOCAL_MAP_HEIGHT - 2;
    
    npc->type = spawn->npcType;
    strcpy(npc->name, spawn->npcName);
    
    // Set safe defaults for all NPCs
    npc->displayChar = 'V';
    npc->color = BROWN;
    strcpy(npc->greeting, "Hello");
    npc->canTalk = true;
    npc->canTrade = false;
    npc->isHostile = false;
    npc->health = 10;
    npc->maxHealth = 10;
    npc->gold = 5;
    npc->state = NPC_IDLE;
    npc->wanderTimer = 60;
    npc->wanderDirection = 0;
    npc->givesQuests = false;
    npc->relationship = 0;
    
    return npc;
}

void UpdateNPCs()
{
    if (!player.isInLocalMap || !npcs) return;
    
    LocalMap* map = worldMap[player.worldPos.y][player.worldPos.x].localMap;
    if (!map) return;
    
    for (int i = 0; i < npcCount; i++) {
        NPC* npc = npcs[i];
        
        // Skip if not in current map or dead
        if (!npc->isInLocalMap || npc->worldX != player.worldPos.x || 
            npc->worldY != player.worldPos.y || npc->state == NPC_DEAD)
            continue;
        
        // Simple wandering AI
        if (npc->state == NPC_IDLE || npc->state == NPC_WANDERING) {
            npc->wanderTimer--;
            
            if (npc->wanderTimer <= 0) {
                if (npc->state == NPC_IDLE) {
                    npc->state = NPC_WANDERING;
                    npc->wanderTimer = 20 + (rand() % 100);
                } else {
                    // Try to move in current direction
                    int newX = npc->localX;
                    int newY = npc->localY;
                    
                    switch (npc->wanderDirection) {
                        case 0: newX++; break;
                        case 1: newX--; break;
                        case 2: newY++; break;
                        case 3: newY--; break;
                    }
                    
                    // Check if move is valid (not a wall and within bounds)
                    if (map && newX > 0 && newX < map->width-1 && 
                        newY > 0 && newY < map->height-1 &&
                        IsTileWalkable(map->tiles[newY][newX])) 
                    {
                        
                        // Also check if too far from spawn point
                        if (npc->spawnPoint) 
                        {
                            int distFromSpawn = abs(newX - npc->spawnPoint->x) + abs(newY - npc->spawnPoint->y);
                            if (distFromSpawn <= 8) { // Max wander distance
                                npc->localX = newX;
                                npc->localY = newY;
                            }
                        } 
                        else 
                        {
                            npc->localX = newX;
                            npc->localY = newY;
                        }
                    }
                    
                    // Change direction sometimes
                    if (rand() % 4 == 0)
                        npc->wanderDirection = rand() % 4;
                    
                    npc->wanderTimer = 20 + (rand() % 100);
                    
                    // Chance to go idle
                    if (rand() % 8 == 0)
                        npc->state = NPC_IDLE;
                }
            }
        }
    }
}

void DrawNPCs()
{
    if (!player.isInLocalMap || !npcs) return;
    
    for (int i = 0; i < npcCount; i++) {
        NPC* npc = npcs[i];
        
        // Only draw if active and in current map
        if (!npc->isActive || !npc->isInLocalMap || 
            npc->worldX != player.worldPos.x || 
            npc->worldY != player.worldPos.y || 
            npc->state == NPC_DEAD)
            continue;

        // DUPLICATE CONDITION - REMOVE THESE LINES
        if (!npc->isInLocalMap || npc->worldX != player.worldPos.x || 
            npc->worldY != player.worldPos.y || npc->state == NPC_DEAD)
            continue;
        
        Vector2 pos = {
            npc->localX * TILE_SIZE + 8.0f,
            npc->localY * TILE_SIZE + 6.0f
        };
        
        DrawTextEx(GetFontDefault(), TextFormat("%c", npc->displayChar),
                  pos, 24, 1, npc->color);
        
        // Show name when nearby
        int distX = abs(npc->localX - player.localPos.x);
        int distY = abs(npc->localY - player.localPos.y);
        if (distX < 6 && distY < 6) {
            Vector2 namePos = {
                npc->localX * TILE_SIZE + 8.0f,
                npc->localY * TILE_SIZE - 20.0f
            };
            
            Color nameColor = npc->isUnique ? GOLD : LIGHTGRAY;
            DrawTextEx(GetFontDefault(), npc->name, namePos, 16, 1, nameColor);
        }
    }
}

void InteractWithNPC()
{
    if (!player.isInLocalMap || !npcs) return;
    
    for (int i = 0; i < npcCount; i++) {
        NPC* npc = npcs[i];
        
        if (!npc->isInLocalMap || npc->worldX != player.worldPos.x || 
            npc->worldY != player.worldPos.y || npc->state == NPC_DEAD)
            continue;
        
        int distX = abs(npc->localX - player.localPos.x);
        int distY = abs(npc->localY - player.localPos.y);
        
        if (distX <= 1 && distY <= 1 && npc->canTalk) {
            // Start conversation
            printf("[%s]: %s\n", npc->name, npc->greeting);
            
            // Different reactions based on relationship/hostility
            if (npc->isHostile) {
                printf("They attack!\n");
                // Combat would start here
            } else if (npc->relationship < -20) {
                printf("They don't look happy to see you.\n");
            } else if (npc->relationship > 50) {
                printf("They smile warmly at you.\n");
            }
            break;
        }
    }
}

void NPCTakeDamage(NPC* npc, int damage)
{
    npc->health -= damage;
    
    if (npc->health <= 0) {
        npc->state = NPC_DEAD;
        NPCDied(npc);
        
        // Death message based on type
        const char* deathMessages[] = {
            " dies.",
            " falls to the ground.",
            " lets out a final cry.",
            " breathes their last.",
            " collapses."
        };
        printf("%s%s\n", npc->name, deathMessages[rand() % 5]);
    } else {
        // Hit reaction
        const char* hitMessages[] = {
            " takes damage!",
            " grunts in pain.",
            " staggers back.",
            " is hit!",
            " cries out!"
        };
        printf("%s%s\n", npc->name, hitMessages[rand() % 5]);
        
        // Chance to flee if low health
        if (npc->health < npc->maxHealth / 3 && rand() % 3 == 0) {
            npc->state = NPC_FLEEING;
            printf("%s tries to flee!\n", npc->name);
        }
    }
}