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