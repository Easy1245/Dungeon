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

typedef struct Monster {
    char name[MAX_NAME];
    int health;
} Monster;

typedef struct Room {
    int id;
    char name[MAX_NAME];
    char description[MAX_DESC];
    struct Room* connections[MAX_CONNECTIONS];
    char* items[MAX_ITEMS];
    Monster* monster;
    bool hasTreasure;
} Room;

typedef struct Player {
    char name[MAX_NAME];
    Room* currentRoom;
    char** inventory;      
    int inventoryCount;    
    int inventoryCapacity; 
    int health;
    int damage;
    int defense;
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

Room* createRoom(int id, const char* name, const char* description) {
    Room* room = malloc(sizeof(Room));
    if (!room) {
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

void addItem(Room* room, const char* itemName) {
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (room->items[i] == NULL) {
            room->items[i] = strdup(itemName);
            return;
        }
    }
}

void connectRooms(Room* a, Room* b) {
    for (int i = 0; i < MAX_CONNECTIONS; i++) 
    {
        if (a->connections[i] == NULL) {
            a->connections[i] = b;
            break;
        }
    }
    for (int i = 0; i < MAX_CONNECTIONS; i++) 
    {
        if (b->connections[i] == NULL) {
            b->connections[i] = a;
            break;
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
        if (r->items[i]) {
            printf("🧺 You see an item: %s\n", r->items[i]);
        }
    }
    if (r->hasTreasure) 
    {
        printf("💰 You see a treasure chest!\n");
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

static void addItemToInventory(Player* player, const char* item) {
    if (player->inventoryCount >= player->inventoryCapacity) {
        int newCapacity = (player->inventoryCapacity == 0) ? 10 : player->inventoryCapacity * 2;
        char** newInventory = realloc(player->inventory, newCapacity * sizeof(char*));
        if (!newInventory) {
            printf(" Error: failed to expand inventory!\n");
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
            if (strcmp(item, "Golden Apple") == 0) {
                int heal = 35;
                player->health += heal;
                if (player->health > 100) player->health = 100;
                printf("🍎 The Golden Apple heals you by %d HP! New health: %d\n", heal, player->health);
            } else if (strcmp(item, "Hyldrul Shield") == 0) {
                printf("🛡️ You feel protected by the Hyldrul Shield.\n");
                player->defense += 10;
            } else if (strcmp(item, "Iron Pickaxe") == 0) {
                printf("⚔️ The Iron Pickaxe feels powerful!\n");
                player->damage += 15;
                printf("Your weapon now does %d base damage.\n", player->damage);
            } else if (strcmp(item, "Excaliber") == 0) {
                printf("⚔️ The Excaliber is a legendary sword!\n");
                player->damage += 50;
                printf("Your weapon now does %d base damage.\n", player->damage);
            } else if (strcmp(item, "Enchanted Golden Apple") == 0) {
                int heal = 75;
                player->health += heal;
                if (player->health > 100) player->health = 100;
                printf("🍏 The Enchanted Golden Apple heals you by %d HP! New health: %d\n", heal, player->health);
            } else if (strcmp(item, "fernandes bottle") == 0) {
                int heal = 30;
                player->health += heal;
                if (player->health > 100) player->health = 100;
                printf("🥤 You picked up a fernandes bottle!\n");
            } else if (strcmp(item, "Enchanted Armor") == 0) {
                printf("🛡️ You feel protected by the Enchanted Armor Shield.\n");
                player->defense += 40;
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

void movePlayer(Player* player, int index) {
    if (index < 0 || index >= MAX_CONNECTIONS || player->currentRoom->connections[index] == NULL) {
        printf("Invalid choice.\n");
        return;
    }

    if (player->currentRoom->id == 10)
    {
        printVisionChamberMap(); 
    }

    Room* nextRoom = player->currentRoom->connections[index];

    if (strcmp(nextRoom->name, "Excalibur Chamber") == 0) {
        bool hasDragonKey = false;
        bool hasRoyalKey = false;

        for (int i = 0; i < player->inventoryCount; i++) {
            if (strcmp(player->inventory[i], "Dragon Key") == 0) hasDragonKey = true;
            if (strcmp(player->inventory[i], "Royal Key") == 0) hasRoyalKey = true;
        }

        if (!hasDragonKey || !hasRoyalKey) {
            printf("🚫 You need both the Dragon Key and the Royal Key to enter the Excalibur Chamber! 🚫\n");
            return;
        }
    }

    player->currentRoom = nextRoom;
    printf("You walk to: %s\n", nextRoom->name);

    if (strcmp(nextRoom->name, "Vision Chamber") == 0) {
        printVisionChamberMap();
    }

    if (nextRoom->monster) {
        fightMonster(player);
    }
}