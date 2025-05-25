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

int getRoomIndex(Room* room) {
    for (int i = 0; i < roomCount; i++) {
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

void connectRooms(Room* a, Room* b) {
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (a->connections[i] == NULL) {
            a->connections[i] = b;
            break;
        }
    }
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (b->connections[i] == NULL) {
            b->connections[i] = a;
            break;
        }
    }
}

void showRoom(const Player* player) {
    Room* r = player->currentRoom;
    printf("\nYou are in: %s\n%s\n", r->name, r->description);
    if (r->monster) {
        printf("⚔️ There is a %s here! (Health: %d)\n", r->monster->name, r->monster->health);
    }
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (r->items[i]) {
            printf("🧺 You see an item: %s\n", r->items[i]);
        }
    }
    if (r->hasTreasure) {
        printf("💰 You see a treasure chest!\n");
    }
    printf("Available exits:\n");
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (r->connections[i]) {
            printf("  [%d] %s\n", i, r->connections[i]->name);
        }
    }
    printf("Type the number to move, 'take' to pick up items, 'save' to save the game, 'load' to load the game, 'q' to quit.\n");
}