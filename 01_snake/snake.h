#ifndef snake_H_
#define snake_H_
#include <stdio.h>

typedef enum ErrorCodes ErrorCodes;
enum ErrorCodes
{
    PARAM_INVALID = 1,
    FILE_OPEN,
    FILE_READ,
    LEVEL_INVALID
};

typedef enum Gamestate Gamestate;
enum Gamestate
{
    GAME_CONTINUE = 4,
    COLLISION_SNAKE,
    COLLISION_WALL
};

typedef struct Level
{
    struct Snakesegment *tail; /* Actually the snakes head. Will be referred to as such in the documentation. */
    struct Snakesegment *head; 
    struct Map *map;
    size_t foodAmount;
    size_t growth;
}Level;

typedef struct Map
{
    size_t size;
    char **data;
}Map;

typedef struct Snakesegment Snakesegment;
struct Snakesegment
{
    int x;
    size_t y;
    struct Snakesegment *next;
};

int dequeue(Level *level);

int detectCollision(FILE *output, Level *level, Snakesegment *tail);

Snakesegment* enqueue(Level *level, int x, size_t y);

int evalInput(FILE *output, Level *level, char *input, size_t *turns);

void freeLevel(Level *level);

int openFile(FILE **f, char *path, char *permission);

void parseInput(Level *level, FILE *inputFile);

Level* parseLevel(FILE *levelFile, FILE *output);

void run(FILE *input, FILE *output, FILE *levelFile);

int printError(int code);

int toInt(char c);

void updateState(FILE *output, Level *level, Snakesegment *tail);

 #endif