#include "snake.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * This is where the magic happens. Responsible for handling command line arguments and passing those to the responsible functions.
 * @param argc - Command line arguments count. Will always be at least 1 (programms name)
 * @param argv - Argument vector that contains all the arguments there have been passed through the command line.
 * @returns Returns one of the error codes in the ErrorCodes enum.
*/
int main(int argc, char **argv)
{
	FILE *input = stdin;
	FILE *output = stdout;
	FILE *levelFile = NULL;
	int error = 0;
	if (argc == 1)
	{
		if((error = openFile(&levelFile, "level/1.txt", "r")) != 0)
        {
            if(levelFile)
                fclose(levelFile);
            return printError(error);
        }
		run(input, output, levelFile);
        fclose(levelFile);
	}
	else if ((argc - 1) % 3 == 0) /* To properly execute the program there must be a set of 3 arguments per execution. */
	{
		for (size_t i = 1; i < argc; i += 3)
		{
			if (*argv[i] != '-')
			{
				if ((error = openFile(&input, argv[i], "r")) != 0)
				{
					if (input && input != stdin)
						fclose(input);
					return printError(error);
				}
			}
			if (*argv[i + 1] != '-')
			{
				if ((error = openFile(&output, argv[i + 1], "w")) != 0)
				{
					if (input != stdin)
						fclose(input);
					return printError(error);
				}
			}
			if (*argv[i + 2] != '-')
			{
				if ((error = openFile(&levelFile, argv[i + 2], "r")) != 0)
				{
					if (input != stdin)
						fclose(input);
					if (output != stdout)
						fclose(output);
					if (levelFile)
						fclose(levelFile);
					return printError(error);
				}
			}
			else levelFile = fopen("level/1.txt", "r");

			run(input, output, levelFile); /* Executes the level with the passed arguments */

            /* Closes File Pointers */
			if (input != stdin)
				fclose(input);
			if (output != stdout)
				fclose(output);
			fclose(levelFile);
            /* Resets to default */
            input = stdin;
			output = stdout;
		}
	}
	else return printError(PARAM_INVALID);

	return 0;
}

/**
 * Run a game of snake. Function containts the main game loop, calling evalInput for every new input.
 * @param input - Filepointer to the control input file.
 * @param output - Filepointer to the control output file.
 * @param levelFile - Filepointer to the level file.
 */
void run(FILE *input, FILE *output, FILE *levelFile)
{
	int buffSize = 82; /* Buffer size including line break */
	char buffer[buffSize];
	size_t turns = 1;

	Level *level = parseLevel(levelFile, output);
	int result = 0;

    /* Main Game Loop */
	while (fgets(buffer, buffSize, input) != NULL && (result = evalInput(output, level, buffer, &turns)) == GAME_CONTINUE);
	if (result == 0)
		fprintf(output, "Gewonnen!\n");
	else if (result == COLLISION_WALL)
		fprintf(output, "Die Schlange ist in die Wand gelaufen!\n");
	else if (result == COLLISION_SNAKE)
		fprintf(output, "Die Schlange hat sich selbst gefressen!\n");
	freeLevel(level);
}

/**
 * Evalutes an input sequence of movement characters and update the level state accordingly.
 * @param output - Filepointer to the output file where the updated level state will be printed. Calls the updateState function to handle that.
 * @param level - Pointer to the level struct of the current level.
 * @param input - The character input sequence.
 * @param turns - A pointer to the turn count. Is increased after every movement key has been evaluated. 
 * @returns Returns one of the game states in the Gamestate enum (see header file).
*/
int evalInput(FILE *output, Level *level, char *input, size_t *turns)
{
	size_t len = strlen(input);
	Snakesegment *tail = NULL;
	unsigned char sContinue = 0;
	for (size_t i = 0; i < len; i++)
	{
		switch (input[i])
		{
			default:
				sContinue = 1;
			break;

			case 'w':
				tail = enqueue(level, level->tail->x, level->tail->y - 1);
			break;

			case 'a':
				tail = enqueue(level, level->tail->x - 1, level->tail->y);
			break;

			case 's':
				tail = enqueue(level, level->tail->x, level->tail->y + 1);
			break;

			case 'd':
				tail = enqueue(level, level->tail->x + 1, level->tail->y);
			break;
		}

		/* Save unncessary instructions */
		if (sContinue)
		{
			sContinue = 0;
			continue;
		}

		/* Print instruction count */
		fprintf(output, "%zu\n", *turns);
		*turns = *turns + 1;

        /* If snake is supposed to grow, don't bother dequeuing the snakes tail. */
		if (level->growth == 0)
			dequeue(level);

		int result = 0;
		if ((result = detectCollision(output, level, tail)) != 0)
			return result;

		updateState(output, level, tail);

        /* GAME WON! */
		if (level->foodAmount == 0)
			return 0;
	}
	return GAME_CONTINUE;
}

/**
 * Updates the state of the map and its snake position. Prints the map to the output stream afterwards.
 * @param output - Filepointer to the output file where the map will be printed to.
 * @param levelFile - Filepointer to the level file.
 * @param tail - Pointer to the snakes tail.
 */
void updateState(FILE *output, Level *level, Snakesegment *tail)
{
	if (tail != NULL)
		level->map->data[tail->y][tail->x] = 'O';

	Map *map = level->map;
	for (int i = 0; i < map->size; i++)
		fprintf(output, "%s", map->data[i]);
}

/**
 * Part of the game logic. Function is being called in every evalInput iteration to check for collision.
 * @param output - Filepointer to the output file. Updated state is printed one last time to the stream on deadly collision x_x
 * @param level - Pointer to the level struct. Used to access game data.
 * @param tail - The snakes head (used for collision detection).
 * @returns Returns one of the game states in the Gamestate enum (see header file).
*/ 
int detectCollision(FILE *output, Level *level, Snakesegment *tail)
{
	if (level->growth > 0)
		level->growth = level->growth - 1;

	/* Check for food collision */
	if (isdigit(level->map->data[tail->y][tail->x]))
	{
		int n = toInt(level->map->data[level->tail->y][level->tail->x]);
		level->foodAmount = level->foodAmount - n;
		level->growth = level->growth + n;
	}

	/* Check for snake segment colission*/
	if (level->map->data[tail->y][tail->x] == 'O')
	{
		updateState(output, level,tail);
		return COLLISION_SNAKE;
	}

	/* Check for wall collision */
	if (level->map->data[level->tail->y][level->tail->x] == 'X')
	{
		updateState(output, level, tail);
		return COLLISION_WALL;
	}

	return 0;
}

/**
 * fopen wrapper to return application specific error codes.
 * @param f - Pointer to the filepointer that needs to be opened.
 * @param path - Filepath string literal
 * @param permission - Permission string literal
 * @returns Returns 0 if file opened properly. Returns 2 if file can't be opened. Returns 3 if no access to read from file.
 */
int openFile(FILE **f, char *path, char *permission)
{
	*f = fopen(path, permission);
	if (!*f)
		return FILE_OPEN; /* Can't open file */

    /* Only check read permissions if read permissions are passed */    
	if (*permission == 'r')
	{
		clearerr(*f); /* Clear errors to avoid false-positives */
		int c = fgetc(*f); /* Reads first character from the stream */
		if (ferror(*f))
		{
			clearerr(*f);
			return FILE_READ; /* No permissions to read file */
		}
		else ungetc(c, *f); /* Push the character back to the stream so it can be read again */
	}
	return 0; /* File opened properly */
}

/**
 * Parses a level from a file to a dynamic string array.
 * @param levelFile - Filepointer to the level file.
 * @param output - Filepointer to the output stream. Prints the parsed map to the output stream.
 * @returns Returns a pointer to the parsed level struct.
*/
Level* parseLevel(FILE *levelFile, FILE *output)
{
	Level *level = (Level*) calloc(1, sizeof(struct Level));
	Map *map = (Map*) calloc(1, sizeof(struct Map));
	int LINE_LENGTH = 82;
	char buffer[LINE_LENGTH];

	/* Allocate memory for first line, so we can reallocate memory for newer lines later */
	if (fgets(buffer, LINE_LENGTH, levelFile) != NULL)
	{
		if (strlen(buffer) < LINE_LENGTH - 1)
		{
			printError(LEVEL_INVALID);
			return NULL;
		}

		map->data = (char**) malloc(sizeof(char*));
		map->data[0] = (char*) malloc(LINE_LENGTH* sizeof(char));
		strcpy(map->data[0], buffer);
		map->size = 1;
	}

	/* Reallocate memory for following line */
	while (fgets(buffer, LINE_LENGTH, levelFile) != NULL)
	{
		if (strlen(buffer) < LINE_LENGTH - 1)
		{
			printError(LEVEL_INVALID);
			return NULL;
		}

		map->data = (char **)realloc(map->data, (map->size + 1) * sizeof(char*));
		map->data[map->size] = malloc(LINE_LENGTH * sizeof(char));
		strcpy(map->data[map->size], buffer);
		map->size = map->size + 1;
	}
	level->map = map;

	/* Parse snake position and count amount of food */
	for (size_t i = 0; i < map->size; i++)
	{
		for (int j = 0; j < LINE_LENGTH; j++)
		{
			if (isdigit(map->data[i][j]))
				level->foodAmount = level->foodAmount + toInt(map->data[i][j]);
			else if (map->data[i][j] == 'O')
				enqueue(level, j, i);
		}
	}
    updateState(output, level, NULL);

	return level;
}

/**
 * Free memory that has been allocated for a level.
 * @param level - A pointer to the level struct. 
*/
void freeLevel(Level *level)
{
	for (size_t i = 0; i < level->map->size; i++)
		free(level->map->data[i]);
	free(level->map->data);
	free(level->map);

	Snakesegment *next = NULL;
	while (level->head != NULL)
	{
		next = level->head->next;
		free(level->head);
		level->head = next;
	}
	free(level);
}

/* -- UTILITY FUNCTIONS --*/

/**
 * Add a new snake segment to the queue. Keep in mind that the snakes head is the queues tail.
 * @param x - The x position of the new segment.
 * @param y - The y Position of the new segment.
 * @returns Returns the snakes new head.
*/
Snakesegment* enqueue(Level *level, int x, size_t y)
{
	Snakesegment *newSegment = (Snakesegment*) calloc(1, sizeof(struct Snakesegment));
	newSegment->x = x;
	newSegment->y = y;

	if (level->head == NULL)
		level->head = newSegment;
	else
		level->tail->next = newSegment;

	level->tail = newSegment;
	return newSegment;
}

/**
 * Remove the first snake segment from the queue. Keep in mind that the snakes head is the queue tail.
 * @param level - Pointer to the level struct. Used to update tail pointer.
 * @returns Returns 0 if segment has been dequeued, reutrns 1 if it was the last segment.
*/
int dequeue(Level *level)
{
	if (level->head == level->tail)
	{
        level->head = level->tail = NULL;
		return 1;
	}

	Snakesegment *tail = level->head;
	level->head = level->head->next;

	level->map->data[tail->y][tail->x] = ' '; /* Clean up the map after dequeueing the snakes old head. */
	free(tail);
	return 0;
}
/**
 * Converts a character digit to its integer counter part. This function DOES NOT check if the character actually is a digit.
 * @returns Returns the integer in the character c.
*/
int toInt(char c)
{
	return c - '0'; /*a little ASCII wizardry */
}

/**
 * Reponsible for connecting an error code to its corresponding error message. Prints the error message to the stderr stream.
 * @returns Returns the error code that has been passed to the function.
*/
int printError(int code)
{
	printf("Fehler Code: %d\nFehler Nachricht: ", code);
	switch (code)
	{
		default:
			fprintf(stderr, "ERROR\n");
			break;

		case PARAM_INVALID:
			fprintf(stderr, "Unpassende Anzahl an Paramtern.\n");
			break;

		case FILE_OPEN:
			fprintf(stderr, "Datei konnte nicht geöffnet werden.\n");
			break;

		case FILE_READ:
			fprintf(stderr, "Die Datei konnte nicht gelesen werden.\n");
			break;

		case LEVEL_INVALID:
			fprintf(stderr, "Level-Datei beinhaltet nicht 80 Zeichen pro Zeile.\n");
			break;
	}
	return code;
}