#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define MAX_NAME 32
#define MAX_DESC 128
#define MAX_CONNECTIONS 4
#define INPUT_SIZE 32 
#define MAX_ITEMS 5  

typedef struct Monster 
{
    char name[MAX_NAME];
    int health;
} Monster;

typedef struct Room 
{
    int id;
    char name[MAX_NAME];
    char description[MAX_DESC];
    struct Room* connections[MAX_CONNECTIONS];
    char* items[MAX_ITEMS];
    Monster* monster;
    bool hasTreasure;
} Room;

typedef struct Player 
{
    char name[MAX_NAME];
    Room* currentRoom;
    char** inventory;      
    int inventoryCount;    
    int inventoryCapacity; 
    int health;
    int damage;
    int defense;
    int shieldDurability; 
} Player;

Room* rooms[22];
int roomCount = 22;

int getRoomIndex(Room* room) 
{
    for (int i = 0; i < roomCount; i++) 
    {
        if (rooms[i] == room) return i;
    }
    return -1;
}

void GameTitle()
{
  printf("Welcome to: Ethernaty: Descent Beyond");
}    

Room* createRoom(int id, const char* name, const char* description) 
{
    Room* room = malloc(sizeof(Room));
    if (!room) 
    {
        perror("Failed to allocate room");
        exit(EXIT_FAILURE);
    }
    room->id = id;
    strncpy(room->name, name, MAX_NAME - 1);
    strncpy(room->description, description, MAX_DESC - 1);
    room->name[MAX_NAME - 1] = '\0';
    room->description[MAX_DESC - 1] = '\0';

    for (int i = 0; i < MAX_CONNECTIONS; i++) room->connections[i] = NULL;
    for (int i = 0; i < MAX_ITEMS; i++) room->items[i] = NULL;
    room->monster = NULL;
    room->hasTreasure = false;

    return room;
}

void connectRooms(Room* a, Room* b) 
{
    for (int i = 0; i < MAX_CONNECTIONS; i++) 
    {
        if (a->connections[i] == NULL) 
        {
            a->connections[i] = b;
            break;
        }
    }
    for (int i = 0; i < MAX_CONNECTIONS; i++) 
    {
        if (b->connections[i] == NULL) 
        {
            b->connections[i] = a;
            break;
        }
    }
}

Monster* createMonster(const char* name, int health) 
{
    Monster* monster = malloc(sizeof(Monster));
    if (!monster) 
    {
        perror("Failed to allocate monster");
        exit(EXIT_FAILURE);
    }
    strncpy(monster->name, name, MAX_NAME - 1);
    monster->name[MAX_NAME - 1] = '\0';
    monster->health = health;
    return monster;
}

void addItem(Room* room, const char* itemName) 
{
    for (int i = 0; i < MAX_ITEMS; i++) 
    {
        if (room->items[i] == NULL) 
        {
            room->items[i] = strdup(itemName);
            return;
        }
    }
}

void showRoom(const Player* player) 
{
    Room* r = player->currentRoom;
    printf("\nYou are in: %s\n%s\n", r->name, r->description);
    if (r->monster) 
    {
        printf("⚔️ There is a %s here! (Health: %d)\n", r->monster->name, r->monster->health);
    }
    for (int i = 0; i < MAX_ITEMS; i++) 
    {
        if (r->items[i]) 
        {
            printf("🧺 You see an item: %s 🧺\n", r->items[i]);
        }
    }
    if (r->hasTreasure) 
    {
        printf("💰 You see a treasure chest! 💰\n");
    }

    printf("Available exits:\n");

    for (int i = 0; i < MAX_CONNECTIONS; i++) 
    {
        if (r->connections[i]) 
        {
            printf("  [%d] %s\n", i, r->connections[i]->name);
        }
    }
    printf("Type the number to move, 'take' to pick up items, 'save' to save the game, 'load' to load the game, 'q' to quit.\n");
}

static void addItemToInventory(Player* player, const char* item) 
{
    if (player->inventoryCount >= player->inventoryCapacity) 
    {
        int newCapacity = (player->inventoryCapacity == 0) ? 10 : player->inventoryCapacity * 2;
        char** newInventory = realloc(player->inventory, newCapacity * sizeof(char*));
        if (!newInventory) 
        {
            printf("Error: failed to expand inventory!\n");
            return;
        }
        player->inventory = newInventory;
        player->inventoryCapacity = newCapacity;
    }
    player->inventory[player->inventoryCount++] = strdup(item);
}

void takeItems(Player* player)
{
    Room* r = player->currentRoom;
    bool found = false;

    for (int i = 0; i < MAX_ITEMS; i++)
    {
        if (r->items[i] != NULL)
        {
            addItemToInventory(player, r->items[i]);
            printf("✅ You picked up: %s ✅\n", r->items[i]);

            const char* item = r->items[i];
            if (strcmp(item, "Golden Apple") == 0) 
            {
                int heal = 35;
                player->health += heal;
                if (player->health > 100) player->health = 100;
                printf("🍏 The Golden Apple heals you by %d HP! New HP: %d 🍏\n", heal, player->health);
            } 
            else if (strcmp(item, "Hyldrul Shield") == 0) 
            {
                printf("🛡️ You feel protected by the Hyldrul Shield. 🛡️\n");
                player->defense += 10;
                player->shieldDurability = 1; 
            } 
            else if (strcmp(item, "Iron Pickaxe") == 0) 
            {
                printf("⚔️ The Iron Pickaxe feels powerful!\n");
                player->damage += 15;
                printf("Your weapon now does %d base damage.\n", player->damage);
            } 
            else if (strcmp(item, "Excaliber") == 0) 
            {
                printf("⛏️ The Excaliber is a legendary sword! ⛏️\n");
                player->damage += 50;
                printf("Your weapon now does %d base damage.\n", player->damage);
            } 
            else if (strcmp(item, "Enchanted Golden Apple") == 0) 
            {
                int heal = 75;
                player->health += heal;
                if (player->health > 100) player->health = 100;
                printf("🍏✨ The Enchanted Golden Apple heals you by %d HP! New health: %d 🍏✨\n", heal, player->health);
            } 
            else if (strcmp(item, "fernandes bottle") == 0) 
            {
                int heal = 30;
                player->health += heal;
                if (player->health > 100) player->health = 100;
                printf("🥤 You picked up a fernandes bottle! 🥤\n");
            } 
            else if (strcmp(item, "Enchanted Armor") == 0) 
            {
                printf("🔰 You feel protected by the Enchanted Armor Shield. 🔰\n");
                player->defense += 40;
                player->shieldDurability = 4;
            }

            free(r->items[i]);
            r->items[i] = NULL;
            found = true;
        }
    }

    if (!found)
    {
        printf("There are no items to pick up.\n");
    }

    if (r->hasTreasure)
    {
        printf("You have found the treasure and won the game!\n");
        exit(0);
    }
}

void printVisionChamberMap()
{
    printf(" Vision Chamber Map:                                                        \n");
    printf("                                                                            \n");
    printf("       Lumbrig ---- Haunted Forrest ---------- Ancient Runes                \n");
    printf("          |                |                        |                       \n");
    printf("          |           Howling Chasm ---------- Storm Catcher                \n");
    printf("          |                                         |                       \n");
    printf("     Snowy Plains ---- Vision Chamber               |                       \n");
    printf("          |                                         |                       \n");
    printf("    The Cursed Mines ---- Cave of Echoes ---- Maze of Bones                 \n");
    printf("          |                                         |                       \n");
    printf("      DragonDen ---- Excalibur Chamber        Poissendeswap                 \n");
    printf("          |                                         |                       \n");
    printf("          |                                         |                       \n");
    printf("          |                                         |                       \n");
    printf("          |                                         |                       \n");
    printf("          |                                    King's Row                   \n");
    printf("          |                                                                 \n");
    printf("     Lava Fissure ---- Hyrule Castle ---- Legacyroom                        \n");
    printf("          |                  |                                              \n");
    printf("      Darkshrine ---- Snake Temple ---- Gates of Sorrow ---- Ethernaty (x)  \n");
    printf("                                              |                             \n");
    printf("                                          Lake of Hope                      \n");
    printf("\n( 'x' = end goal )                                                        \n");
}

void fightMonster(Player* player) {
    Monster* monster = player->currentRoom->monster;
    if (!monster) return;

    printf("\n⚔️ Combat with %s started!\n", monster->name);

    while (monster->health > 0 && player->health > 0) 
    {
        printf("Your HP: %d | %s HP: %d\n", player->health, monster->name, monster->health);
        printf("Type 'a' to attack or 'r' to run: ");

        char action[INPUT_SIZE];
        if (!fgets(action, sizeof(action), stdin)) break;

        if (action[0] == 'a') {
            int damageToMonster = rand() % 11 + player->damage;  
            int damageToPlayer = rand() % 16 + 5; 
            int mitigatedDamage = damageToPlayer - player->defense;
            if (mitigatedDamage < 0) mitigatedDamage = 0;

            if (player->shieldDurability > 0) 
            {
                printf("👹 Monster attacks for %d damage", damageToPlayer);
                if (player->defense > 0)
                    printf(" but your shield reduces it to %d!\n", mitigatedDamage);
                else
                    printf("!\n");

                player->health -= mitigatedDamage;
                monster->health -= damageToMonster;

                player->shieldDurability--;

                if (player->shieldDurability == 0) 
                {
                    printf("🛡️ Your shield has broken!\n");
                    player->defense -= 10; 
                    if (player->defense < 0) player->defense = 0;
                }
            } 
            else 
            {
                printf("👹 Monster attacks for %d damage!\n", damageToPlayer);
                player->health -= damageToPlayer;
                monster->health -= damageToMonster;
            }

            printf(" You hit the %s for %d damage!\n", monster->name, damageToMonster);
            if (monster->health > 0) 
            {
                printf(" %s hits you for %d damage!\n", monster->name, damageToPlayer);
            }
        } 
        else if (action[0] == 'r') 
        {
            printf("🏃‍♂️ You run back to the previous room!\n");
            for (int i = 0; i < MAX_CONNECTIONS; i++) 
            {
                Room* back = player->currentRoom->connections[i];
                if (back && back->monster == NULL) 
                {
                    player->currentRoom = back;
                    return;
                }
            }
            printf("You can't run from here!\n");
        } 
        else 
        {
            printf(" Invalid action.\n");
        }
    }

    if (player->health <= 0) 
    {
        printf("💀 Hero was slained by %s...\n Game Over. 💀\n", monster->name);
        exit(0);
    } 
    else 
    {
        printf("You defeated the %s!\n", monster->name);
        free(player->currentRoom->monster);
        player->currentRoom->monster = NULL;
    }
}

void movePlayer(Player* player, int index) 
{
    if (index < 0 || index >= MAX_CONNECTIONS || player->currentRoom->connections[index] == NULL) 
    {
        printf("Invalid choice.\n");
        return;
    }

    if (player->currentRoom->id == 10)
    {
        printVisionChamberMap(); 
    }

    Room* nextRoom = player->currentRoom->connections[index];

    if (strcmp(nextRoom->name, "Excalibur Chamber") == 0) 
    {
        bool hasDragonKey = false;
        bool hasRoyalKey = false;

        for (int i = 0; i < player->inventoryCount; i++) 
        {
            if (strcmp(player->inventory[i], "Dragon Key") == 0) hasDragonKey = true;
            if (strcmp(player->inventory[i], "Royal Key") == 0) hasRoyalKey = true;
        }

        if (!hasDragonKey || !hasRoyalKey) 
        {
            printf("🚫 You need both the Dragon Key and the Royal Key to enter the Excalibur Chamber! 🚫\n");
            return;
        }
    }

    player->currentRoom = nextRoom;
    printf("You walk to: %s\n", nextRoom->name);

    if (strcmp(nextRoom->name, "Vision Chamber") == 0) 
    {
        printVisionChamberMap();
    }

    if (nextRoom->monster) 
    {
        fightMonster(player);
    }
}

bool saveGame(const char* filename, Player* player) 
{
    FILE* f = fopen(filename, "wb");
    if (!f) 
    {
        printf("Error opening save file.\n");
        return false;
    }

    fwrite(player->name, sizeof(char), MAX_NAME, f);
    int currentRoomIndex = getRoomIndex(player->currentRoom);
    fwrite(&currentRoomIndex, sizeof(int), 1, f);
    fwrite(&player->health, sizeof(int), 1, f);
    fwrite(&player->damage, sizeof(int), 1, f);
    fwrite(&player->defense, sizeof(int), 1, f);
    fwrite(&player->shieldDurability, sizeof(int), 1, f); 
    fwrite(&player->inventoryCount, sizeof(int), 1, f);

    for (int i = 0; i < player->inventoryCount; i++) 
    {
        size_t len = strlen(player->inventory[i]) + 1;
        fwrite(&len, sizeof(size_t), 1, f);
        fwrite(player->inventory[i], sizeof(char), len, f);
    }

    for (int i = 0; i < roomCount; i++) 
    {
        for (int j = 0; j < MAX_ITEMS; j++) 
        {
            bool hasItem = (rooms[i]->items[j] != NULL);
            fwrite(&hasItem, sizeof(bool), 1, f);
            if (hasItem) 
            {
                size_t len = strlen(rooms[i]->items[j]) + 1;
                fwrite(&len, sizeof(size_t), 1, f);
                fwrite(rooms[i]->items[j], sizeof(char), len, f);
            }
        }
        bool hasMonster = (rooms[i]->monster != NULL);
        fwrite(&hasMonster, sizeof(bool), 1, f);
        if (hasMonster) 
        {
            fwrite(&rooms[i]->monster->health, sizeof(int), 1, f);
        }
    }

    for (int i = 0; i < roomCount; i++) 
    {
        fwrite(&rooms[i]->hasTreasure, sizeof(bool), 1, f);
    }

    fclose(f);
    printf("Game successfully saved.\n");
    return true;
}

bool loadGame(const char* filename, Player* player) 
{
    FILE* f = fopen(filename, "rb");
    if (!f) 
    {
        printf("Error opening save file.\n");
        return false;
    }

    for (int i = 0; i < player->inventoryCount; i++) 
    {
        free(player->inventory[i]);
    }
    free(player->inventory);
    player->inventory = NULL;
    player->inventoryCount = 0;
    player->inventoryCapacity = 0;

    fread(player->name, sizeof(char), MAX_NAME, f);
    int currentRoomIndex;
    fread(&currentRoomIndex, sizeof(int), 1, f);
    if (currentRoomIndex >= 0 && currentRoomIndex < roomCount) 
    {
        player->currentRoom = rooms[currentRoomIndex];
    } 
    else 
    {
        fclose(f);
        printf("Invalid saved room index.\n");
        return false;
    }
    fread(&player->health, sizeof(int), 1, f);
    fread(&player->damage, sizeof(int), 1, f);
    fread(&player->defense, sizeof(int), 1, f);
    fread(&player->shieldDurability, sizeof(int), 1, f); 

    int invCount = 0;
    fread(&invCount, sizeof(int), 1, f);
    player->inventoryCapacity = invCount > 10 ? invCount : 10;
    player->inventory = malloc(player->inventoryCapacity * sizeof(char*));
    player->inventoryCount = 0;
    for (int i = 0; i < invCount; i++) 
    {
        size_t len;
        fread(&len, sizeof(size_t), 1, f);
        char* item = malloc(len);
        fread(item, sizeof(char), len, f);
        player->inventory[player->inventoryCount++] = item;
    }

    for (int i = 0; i < roomCount; i++) 
    {
        for (int j = 0; j < MAX_ITEMS; j++) 
        {
            bool hasItem;
            fread(&hasItem, sizeof(bool), 1, f);
            if (rooms[i]->items[j]) {
                free(rooms[i]->items[j]);
                rooms[i]->items[j] = NULL;
            }
            if (hasItem) {
                size_t len;
                fread(&len, sizeof(size_t), 1, f);
                rooms[i]->items[j] = malloc(len);
                fread(rooms[i]->items[j], sizeof(char), len, f);
            }
        }
        bool hasMonster;
        fread(&hasMonster, sizeof(bool), 1, f);
        if (hasMonster) {
            if (!rooms[i]->monster) rooms[i]->monster = malloc(sizeof(Monster));
            fread(&rooms[i]->monster->health, sizeof(int), 1, f);
        } 
        else 
        {
            if (rooms[i]->monster) 
            {
                free(rooms[i]->monster);
                rooms[i]->monster = NULL;
            }
        }
    }

    for (int i = 0; i < roomCount; i++) 
    {
        fread(&rooms[i]->hasTreasure, sizeof(bool), 1, f);
    }

    fclose(f);
    printf("Game successfully loaded.\n");
    return true;
}

int main()
{

    GameTitle();

    srand((unsigned int)time(NULL));

    rooms[0] = createRoom(0, "Lumbrig", "The humble start of your adventure.");
    rooms[1] = createRoom(1, "Darkshrine", "A creepy shrine full of dark energy.");
    rooms[2] = createRoom(2, "Dragonden", "A huge cave with the roar of dragons.");
    rooms[3] = createRoom(3, "Poissendeswap", "A stinky swamp full of poison.");
    rooms[4] = createRoom(4, "Legacyroom", "A room full of old symbols.");
    rooms[5] = createRoom(5, "Lake of Hope", "A serene glow of hope.");
    rooms[6] = createRoom(6, "Lava Fissure", "A glowing fissure filled with lava.");
    rooms[7] = createRoom(7, "King's Row", "A royal hall with torn banners.");
    rooms[8] = createRoom(8, "Snowy Plains", "An endless snowy plain.");
    rooms[9] = createRoom(9, "Hyrule Castle", "A majestic castle.");
    rooms[10] = createRoom(10, "Vision Chamber", "A mystical room that reveals paths.");
    rooms[11] = createRoom(11, "Excalibur Chamber", "A holy room with the legendary sword Excalibur.");
    rooms[12] = createRoom(12, "Maze of Bones", "A complicated maze filled with bones.");
    rooms[13] = createRoom(13, "Gates of Sorrow", "Gates that emit sorrowful voices.");
    rooms[14] = createRoom(14, "Cave of Echoes", "A cave where sounds echo endlessly.");
    rooms[15] = createRoom(15, "Howling Chasm", "A deep chasm full of howling winds.");
    rooms[16] = createRoom(16, "The Cursed Mines", "Mine shafts cursed by ancient spirits.");
    rooms[17] = createRoom(17, "Ancient Runes", "A place covered with mysterious runes.");
    rooms[18] = createRoom(18, "Snake Temple", "A temple dedicated to snakes.");
    rooms[19] = createRoom(19, "Haunted Forest", "A forest full of ghostly sounds.");
    rooms[20] = createRoom(20, "Storm Catcher", "A stormy labertory to do research on storms");
    rooms[21] = createRoom(21, "Eternity, the End", "A place where everything ends.");

    connectRooms(rooms[0], rooms[19]);
    connectRooms(rooms[0], rooms[8]);
    connectRooms(rooms[19], rooms[15]);
    connectRooms(rooms[19], rooms[17]);
    connectRooms(rooms[17], rooms[20]);
    connectRooms(rooms[15], rooms[20]);
    connectRooms(rooms[20], rooms[12]);
    connectRooms(rooms[8], rooms[10]);
    connectRooms(rooms[8], rooms[16]);
    connectRooms(rooms[16], rooms[14]);
    connectRooms(rooms[16], rooms[2]);
    connectRooms(rooms[14], rooms[12]);
    connectRooms(rooms[12], rooms[3]);
    connectRooms(rooms[3], rooms[7]);
    connectRooms(rooms[2], rooms[11]);
    connectRooms(rooms[2], rooms[6]);
    connectRooms(rooms[6], rooms[9]);
    connectRooms(rooms[6], rooms[1]);
    connectRooms(rooms[9], rooms[18]);
    connectRooms(rooms[9], rooms[4]);
    connectRooms(rooms[1], rooms[18]);
    connectRooms(rooms[18], rooms[13]);
    connectRooms(rooms[13], rooms[5]);
    connectRooms(rooms[13], rooms[21]);

    addItem(rooms[1], "fernandes bottle");
    addItem(rooms[2], "Dragon Key");
    addItem(rooms[4], "Enchanted Armor");
    addItem(rooms[5], "Enchanted Golden Apple");
    addItem(rooms[6], "Golden Apple");
    addItem(rooms[7], "Royal Key");
    addItem(rooms[9], "Hyldrul Shield"); 
    addItem(rooms[10], "Golden Apple");
    addItem(rooms[11], "Excaliber");
    addItem(rooms[13], "PokeBall ");
    addItem(rooms[14], "Iron Pickaxe");
    addItem(rooms[16], "Golden Apple");
    addItem(rooms[17], "Rune Stone");
    addItem(rooms[18], "Golden Apple");
    addItem(rooms[20], "Rusty Key");
    
    rooms[1]->monster = createMonster("Dark Spirit", 50);
    rooms[2]->monster = createMonster("Fire Dragon", 80);
    rooms[3]->monster = createMonster("Toxic Slime", 35);
    rooms[14]->monster = createMonster("Echo Ghost", 25);
    rooms[16]->monster = createMonster("Cursed Miner", 30);
    rooms[17]->monster = createMonster("Rune Sentinel", 45);
    rooms[18]->monster = createMonster("Seviper", 75);
    rooms[19]->monster = createMonster("Haunted Wraith", 40);
    rooms[20]->monster = createMonster("Mad Storm scientis", 20);
    rooms[21]->monster = createMonster("Slifer The Sky Dragon", 350);

    rooms[21]->hasTreasure = true;

    Player player;
    strncpy(player.name, "Hero", MAX_NAME);
    player.health = 100;
    player.damage = 10;
    player.inventoryCount = 0;
    player.inventoryCapacity = 0;
    player.inventory = NULL; 
    player.currentRoom = rooms[0];
    player.defense = 0;
    player.shieldDurability = 0;

    char input[INPUT_SIZE];
    while (true)
    {
        showRoom(&player);

        if (!fgets(input, sizeof(input), stdin)) break;

        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "q") == 0) {
            printf("Game exited.\n");
            break;
        } 
        else if (strcmp(input, "take") == 0) 
        {
            takeItems(&player);
        } 
        else if (strcmp(input, "save") == 0) 
        {
            saveGame("savegame.dat", &player); 
        } 
        else if (strcmp(input, "load") == 0) 
        {
            if (!loadGame("savegame.dat", &player)) 
            {
                printf("Failed to load savegame.\n");
            }
        } else 
        {
            int choice = atoi(input);
            movePlayer(&player, choice);
        }
    }

    for (int i = 0; i < player.inventoryCount; i++) 
    {
        free(player.inventory[i]);
    }
    free(player.inventory);

    for (int i = 0; i < roomCount; i++) 
    {
        for (int j = 0; j < MAX_ITEMS; j++) 
        {
            if (rooms[i]->items[j]) free(rooms[i]->items[j]);
        }
        if (rooms[i]->monster) free(rooms[i]->monster);
        free(rooms[i]);
    }

    return 0;
}

