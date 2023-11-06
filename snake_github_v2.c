
// Earl James Williams Aliñgasa (NeoTSZ on Github)

/*============================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <windows.h>

/*============================================================*/

void runGame(int *cells, int gridWidth);
void renderGrid(int *cells, int gridWidth);
void moveSnake(int *cells, int *snake, int velocity, int gridWidth, int snakeSize);

/*============================================================*/

int main() {
  int gridWidth = 7,
  menuChoice,
  *cells = (int*) calloc(gridWidth * gridWidth, sizeof(int));

  newMenu:
    // Getting menu choice
    printf("\n:: SNAKE ::\n");
    printf("\nChoose an option below by entering the corresponding number.\n");
    printf("\n(1) Start Game");
    printf("\n(2) Change Grid Size");
    printf("\n(Other) Exit Game\n\n%c ", 175);
    scanf("%d", &menuChoice);

    // Processing menu choice
    switch (menuChoice) {
      // Running game
      case 1:
        runGame(cells, gridWidth);
        break;

      // Changing grid size
      case 2:
        // Getting and validating input
        system("cls");
        printf("\n:: CHANGE GRID SIZE ::\n");
        printf("\nEnter a grid width - must be an odd number. (7 - 13)\n\n%c ", 175);
        badWidth:
          scanf("%d", &gridWidth);
          if (gridWidth < 7 || gridWidth > 13 || (gridWidth % 2 == 0)) {
            printf("\n:: Invalid width - Please try again ::\n\n%c ", 175);
            goto badWidth;
          }
        cells = (int*) realloc(cells, (gridWidth * gridWidth) * sizeof(int));
        break;

      // Exiting game
      default:
        printf("\n:: EXITING GAME ::\n\n");
        free(cells);
        return 0;
    }

    system("cls");
    goto newMenu;

  free(cells);
  return 0;
}

/*============================================================*/

void runGame(int *cells, int gridWidth) {
  int velocity = 0,
  snakeSize = 3,
  food,
  *snake = (int*) calloc(snakeSize, sizeof(int));

  // Initializing cells
  for (int i = 0; i < (gridWidth * gridWidth); i++)
    cells[i] = 0;

  // Initializing snake
  for (int i = 0; i < snakeSize; i++) {
    snake[i] = (gridWidth * gridWidth) / 2 + (gridWidth * i);
    cells[snake[i]] = 1;
  }

  // Setting random seed
  srand(time(0));

  // Initializing food
  newFood:
    food = rand() % (gridWidth * gridWidth);
    for (int i = 0; i < snakeSize; i++) {
      if (food == snake[i])
        goto newFood;
    }
  cells[food] = 2;

  printf("\33[?25l");
  renderGrid(cells, gridWidth);

  // Waiting for input
  printf("\n\n[Click ANY movement key to begin.]");

  // Looping game
  while (1) {
    fflush(stdin); // MIGHT BREAK HERE

    printf("\33[?25l");

    if (_kbhit()) {
      switch (getch()) {
        case 'w': case '5': // Up
          if (velocity != 2)
            velocity = 5;
          break;
        case 'a': case '1': // Left
          if (velocity != 3)
            velocity = 1;
          break;
        case 's': case '2': // Down
          if (velocity != 5)
            velocity = 2;
          break;
        case 'd': case '3': // Right
          if (velocity != 1)
            velocity = 3;
          break;
        case 'r': // End game
          free(snake);
          printf("\33[?25h");
          return;
      }
    }

    if (velocity) {
      moveSnake(cells, snake, velocity, gridWidth, snakeSize);

      if (food == snake[0]) {
        snakeSize++;
        snake = (int*) realloc(snake, snakeSize * sizeof(int));
        snake[snakeSize - 1] = snake[snakeSize - 2];

        resetFood:
          food = rand() % (gridWidth * gridWidth);
          for (int i = 0; i < (snakeSize - 1); i++) {
            if (food == snake[i])
              goto resetFood;
          }
        cells[food] = 2;
      }

      // Checking end game state
      for (int i = 1; i < snakeSize; i++) {
        if (snake[0] == snake[i]) {
          printf("\33[?25l");
          cells[snake[0]] = 3;
          renderGrid(cells, gridWidth);
          printf("\n\n:: Game Over! ::\n\n[Final Score: %d]\n", snakeSize - 3);
          printf("\nPress ANY key to return to the main menu.");
          getch();
          free(snake);
          printf("\33[?25h");
          return;
        }
      }

      renderGrid(cells, gridWidth);
      Sleep(100);
    }
  }
}

/*============================================================*/

void renderGrid(int *cells, int gridWidth) {
  int index = 0;

  system("cls");

  // Printing top border
  printf("\n%c%c", 201, 205);
  for (int i = 0; i < gridWidth; i++)
    printf("%c%c", 205, 205);
  printf("%c\n", 187);

  // Printing middle lines
  for (int i = 0; i < gridWidth; i++) {
    printf("%c ", 186);

    // Printing cells
    for (int j = 0; j < gridWidth; j++) {
      switch (cells[index]) {
        // Snake body
        case 1:
          printf("%c ", 254);
          break;

        case 2:
          printf("%c ", 3);
          break;

        case 3:
          printf("X ");
          break;

        // Empty spaces
        default:
          printf("%c ", 250);
      }

      index++;
    }

    printf("%c\n", 186);
  }

  // Printing bottom border
  printf("%c%c", 200, 205);
  for (int i = 0; i < gridWidth; i++)
    printf("%c%c", 205, 205);
  printf("%c", 188);
}

/*============================================================*/

void moveSnake(int *cells, int *snake, int velocity, int gridWidth, int snakeSize) {
  int head = snake[0];
  cells[snake[snakeSize - 1]] = 0;

  // Updating body
  for (int i = (snakeSize - 1); i > 0; i--)
    snake[i] = snake[i - 1];

  switch (velocity) {
    case 5: // Up
      (head < gridWidth) ? (head += (gridWidth * (gridWidth - 1))) : (head -= gridWidth);
      break;
    case 1: // Left
      (head % gridWidth == 0) ? (head += (gridWidth - 1)) : (head--);
      break;
    case 2: // Down
      (head >= (gridWidth * (gridWidth - 1))) ? (head -= (gridWidth * (gridWidth - 1))) : (head += gridWidth);
      break;
    case 3: // Right
      (head % gridWidth == (gridWidth - 1)) ? (head -= (gridWidth - 1)) : (head++);
      break;
  }

  snake[0] = head;
  cells[head] = 1; // TEMPORARY
}