// Developed by Earl James Williams Aliñgasa (NeoTSZ on GitHub).

/*============================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>

/*============================================================*/

// Size macros
#define GRID_WIDTH          11
#define START_SNAKE_SIZE    3
#define CELL_COUNT          (GRID_WIDTH * GRID_WIDTH)
#define LINE_CHAR_COUNT     ((GRID_WIDTH * 2) + 2)
#define VERTICAL_JUMP       (CELL_COUNT - GRID_WIDTH)
#define HORIZONTAL_JUMP     (GRID_WIDTH - 1)

/*============================================================*/

// Setup macros
#define CSI(X)              "\x1B[" #X      // Control Sequence Introducer
#define FCS(X)              CSI(38;5;) #X   // Foreground Color Setter

/*============================================================*/

// Color macros
#define RESET_COLOR         CSI(0m)
#define GREY                FCS(8m)
#define RED                 FCS(9m)
#define GREEN               FCS(10m)
#define YELLOW              FCS(11m)
#define BLUE                FCS(12m)
#define CYAN                FCS(14m)

/*============================================================*/

// Cursor macros
#define GO_UP(X)            CSI(X) "A"
#define GO_DOWN(X)          CSI(X) "B"
#define GO_RIGHT(X)         CSI(X) "C"
#define GO_LEFT(X)          CSI(X) "D"
#define RESET_CURSOR        CSI(2;3H)
#define SAVE_CURSOR         CSI(s)
#define LOAD_CURSOR         CSI(u)

/*============================================================*/

// Printing macros
#define CLEAR_SCREEN        CSI(2J) CSI(H)
#define HIDE_CURSOR         CSI(?25l)
#define SHOW_CURSOR         CSI(?25h)
#define DRAW_CELL           GREY "\xFA" GO_LEFT(1)
#define DRAW_FOOD           RED "@" GO_LEFT(1)
#define DRAW_BODY           GREEN "\xFE" GO_LEFT(1)
#define DRAW_HEAD           YELLOW "\xFE" GO_LEFT(1)
#define DRAW_DEAD_PART      BLUE "\xFE" GO_LEFT(1)
#define SHOW_BUTTON(X,Y)    CYAN "[" #X "]\xFF" RESET_COLOR #Y

/*============================================================*/

typedef struct GameState
{
    int cells[CELL_COUNT];  // Holds the value at each cell
    int snake[CELL_COUNT];  // Holds the positions of each snake body part
    int length;             // Length of the snake
    int food;               // Position of the food
    int direction;          // Direction in which the snake moves
    struct timespec timer;  // Used to control frame times
} GameState;

/*============================================================*/

void draw_grid()
{
    char borderLine[LINE_CHAR_COUNT];
    char cellDots[LINE_CHAR_COUNT];
    int arrayIndex;

    // \xBA - left/right border
    // \xBB - top right corner
    // \xBC - bottom right corner
    // \xC8 - bottom left corner
    // \xC9 - top left corner
    // \xCD - top/bottom border
    // \xFA - cell dot
    // \xFE - snake body part
    // \xFF - non-breaking space

    // Setting up the character arrays borderLine[] and cellDots[]
    for (arrayIndex = 0; arrayIndex < (LINE_CHAR_COUNT - 1); arrayIndex ++)
    {
        *(borderLine + arrayIndex) = '\xCD';
        *(cellDots + arrayIndex) = (arrayIndex % 2 == 0) ? '\xFF' : '\xFA';
    }
    *(borderLine + arrayIndex) = '\0';
    *(cellDots + arrayIndex) = '\0';

    // Printing the top border and saving the cursor position
    printf(HIDE_CURSOR RESET_COLOR CLEAR_SCREEN "\xC9%s\xBB", borderLine);
    printf("\xFF" SAVE_CURSOR "\n");

    // Printing the middle of the grid
    for (int i = 0; i < GRID_WIDTH; i ++)
    {
        printf("\xBA" GREY "%s" RESET_COLOR "\xBA\n", cellDots);
    }

    // Printing the bottom border
    printf("\xC8%s\xBC", borderLine);

    // Printing the controls
    printf(RESET_COLOR LOAD_CURSOR GO_DOWN(1) SAVE_CURSOR);
    printf("CONTROLS:" LOAD_CURSOR GO_DOWN(1) SAVE_CURSOR);
    printf(SHOW_BUTTON(W/5, Up) LOAD_CURSOR GO_DOWN(1) SAVE_CURSOR);
    printf(SHOW_BUTTON(S/2, Down) LOAD_CURSOR GO_DOWN(1) SAVE_CURSOR);
    printf(SHOW_BUTTON(D/3, Right) LOAD_CURSOR GO_DOWN(1) SAVE_CURSOR);
    printf(SHOW_BUTTON(A/1, Left) LOAD_CURSOR GO_DOWN(1) SAVE_CURSOR);
    printf(SHOW_BUTTON(E, Exit) RESET_CURSOR);

    return;
}

/*============================================================*/

void draw_at_position(int cellValue, int position)
{
    // Getting the row and column associated with the position on the grid
    int row = position / GRID_WIDTH;
    int column = position % GRID_WIDTH;

    // Moving the cursor to the position
    printf(RESET_CURSOR);
    for (int i = 0; i < row; i ++)
    {
        printf(GO_DOWN(1));
    }
    for (int i = 0; i < column; i ++)
    {
        printf(GO_RIGHT(2));
    }

    // Printing the character associated with cellValue
    switch (cellValue)
    {
    case 0: // Empty cell
        printf(DRAW_CELL);
        break;

    case 1: // Snake head
        printf(DRAW_HEAD);
        break;

    case 2: // Snake body part
        printf(DRAW_BODY);
        break;

    case 3: // Food piece
        printf(DRAW_FOOD);
        break;

    case 4: // Dead snake body part
        printf(DRAW_DEAD_PART);
        break;
    }

    return;
}

/*============================================================*/

void put_food(GameState* game)
{
    int foodPosition;

    // Randomly choosing cells until an empty cell is found
    while (1)
    {
        foodPosition = rand() % 81;
        if (*(game->cells + foodPosition) == 0)
        {
            *(game->cells + foodPosition) = 3;
            game->food = foodPosition;
            draw_at_position(3, foodPosition);

            return;
        }
    }
}

/*============================================================*/

void initialize_game(GameState* game)
{
    draw_grid();

    // Setting up the randomizer and timer
    srand(time(0));
    game->timer.tv_sec = 0;
    game->timer.tv_nsec = 100000000;

    // Initializing the cells
    for (int i = 0; i < CELL_COUNT; i ++)
    {
        *(game->cells + i) = 0;
    }

    // Initializing the snake
    game->length = START_SNAKE_SIZE;
    game->direction = 0;
    for (int i = 0; i < START_SNAKE_SIZE; i ++)
    {
        int tempPosition = (CELL_COUNT / 2) + (i * GRID_WIDTH);
        *(game->snake + i) = tempPosition;
        *(game->cells + tempPosition) = (i == 0) ? 1 : 2;
    }

    // Putting the snake on the grid
    draw_at_position(1, *(game->snake));
    for (int i = 1; i < START_SNAKE_SIZE; i ++)
    {
        draw_at_position(2, *(game->snake + i));
    }

    // Putting the food on the grid
    put_food(game);

    return;
}

/*============================================================*/

void play_death_animation(GameState* game)
{
    // Making the snake blue
    game->timer.tv_nsec = 50000000;
    for (int i = 0; i < game->length; i ++)
    {
        draw_at_position(4, *(game->snake + i));
        nanosleep(&(game->timer), NULL);
    }

    // Showing outro message
    printf(RESET_COLOR RESET_CURSOR);
    for (int i = 0; i <= GRID_WIDTH; i ++)
    {
        printf(GO_DOWN(1));
    }
    printf("\nYour score is " GREEN "%d" RESET_COLOR "!\n", (game->length - 3));

    // Prompting the user to close the terminal
    printf("\nPress ENTER to close the game.\n");
    fflush(stdin);
    getchar();

    exit(0);
}

/*============================================================*/

void process_input(char input, GameState* game)
{
    switch (input)
    {
    case 'w': case 'W': case '5':   // Move up
        game->direction = (game->direction != 2) ? 5 : game->direction;
        break;

    case 's': case 'S': case '2':   // Move down
        game->direction = (game->direction != 5) ? 2 : game->direction;
        break;

    case 'd': case 'D': case '3':   // Move right
        game->direction = (game->direction != 1) ? 3 : game->direction;
        break;

    case 'a': case 'A': case '1':   // Move left
        game->direction = (game->direction != 3) ? 1 : game->direction;
        break;

    case 'e': case 'E':             // Exit the game
        play_death_animation(game);
    }

    return;
}

/*============================================================*/

void update_snake(GameState* game)
{
    int head = *(game->snake);
    int tail = *(game->snake + game->length - 1);

    // Removing the head and tail
    *(game->cells + head) = 2;
    *(game->cells + tail) = 0;
    draw_at_position(2, head);
    draw_at_position(0, tail);

    // Updating the body
    for (int i = (game->length - 1); i > 0; i --)
    {
        *(game->snake + i) = *(game->snake + i - 1);
    }

    // Updating the head's location
    switch (game->direction)
    {
    case 5: // Move up
        head = (head < GRID_WIDTH) ? head + VERTICAL_JUMP : head - GRID_WIDTH;
        break;

    case 2: // Move down
        head = (head < VERTICAL_JUMP) ? head + GRID_WIDTH : head - VERTICAL_JUMP;
        break;

    case 3: // Move right
        head = (head % GRID_WIDTH == HORIZONTAL_JUMP) ? head - HORIZONTAL_JUMP : head + 1;
        break;

    case 1: // Move left
        head = (head % GRID_WIDTH == 0) ? head + HORIZONTAL_JUMP : head - 1;
        break;
    }

    // Saving the current value of the cell before overriding it
    int valueAtHeadCell = *(game->cells + head);

    // Printing the head onto the grid
    *(game->snake) = head;
    *(game->cells + head) = 1;
    draw_at_position(1, head);

    // Checking if the food was eaten
    if (head == game->food)
    {
        game->length += 1;
        *(game->snake + game->length - 1) = tail;
        *(game->cells + tail) = 1;
        put_food(game);
        draw_at_position(2, tail);
    }

    // Checking if the snake eats itself
    if (valueAtHeadCell == 2)
    {
        process_input('e', game);
    }

    nanosleep(&(game->timer), NULL);

    return;
}

/*============================================================*/

int main(void)
{
    GameState game;

    initialize_game(&game);

    // Looping the game
    while (1)
    {
        // Checking for user input
        if (_kbhit())
        {
            process_input(getch(), &game);
        }

        // Updating the snake's body on the grid if it is moving
        if (game.direction)
        {
            update_snake(&game);
        }
    }

    return 0;
}
