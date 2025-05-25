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
