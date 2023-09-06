
// Earl james Williams Aliñgasa (NeoTSZ on Github)

// Inspiration comes from this video: 
// https://www.youtube.com/watch?v=cUJE10XEjrU

/*============================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <conio.h>
#include <time.h>

/*============================================================*/

int side_length = 9;

/*============================================================*/

void renderGrid(int cells[]);
void updateSnake(int cells[], int snake[], int velocity, int size);

/*============================================================*/

int main() {
  int totalCellCount = side_length * side_length,
  cells[totalCellCount],
  quit = 0,
  velocity = 0,
  snake[totalCellCount],
  size = 3,
  food;

  srand(time(0));

  // Initializing cells
  for (int i = 0; i < totalCellCount; i++) cells[i] = 0;

  // Initializing snake
  for (int i = 0; i < totalCellCount; i++) snake[i] = totalCellCount / 2;
  for (int i = 0; i < size; i++) {
    snake[i] = (i * side_length) + (totalCellCount / 2);
    cells[snake[i]] = 2;
  }
  cells[snake[0]] = 1;

  // Initializing food
  resetFood1:
  food = rand() % totalCellCount;
  for (int i = 0; i < size; i++) {
    if (food == snake[i]) goto resetFood1;
  }
  cells[food] = 3;

  printf("\33[?25l");
  renderGrid(cells);

  while (!quit) {
    if (_kbhit()) {
      switch (getch()) {
        case '1': // Left
          if (velocity != 3) velocity = 1;
          break;
        case '2': // Down
          if (velocity != 5) velocity = 2;
          break;
        case '3': // Right
          if (velocity != 1) velocity = 3;
          break;
        case '5': // Up
          if (velocity != 2) velocity = 5;
          break;
      }
    }

    updateSnake(cells, snake, velocity, size);
    if (velocity) renderGrid(cells);

    // Checking if food is eaten
    if (food == snake[0]) {
      resetFood2:
      food = rand() % totalCellCount;
      for (int i = 0; i < size; i++) {
        if (food == snake[i]) goto resetFood2;
      }
      cells[food] = 3;
      size++;
      renderGrid(cells);
    }

    // Checking if snake eats itself
    for (int i = 1; i < size; i++) {
      if (snake[0] == snake[i]) {
        quit = 1;
        cells[snake[0]] = 4;
        renderGrid(cells);
        break;
      }
    }
    usleep(100000);
  }

  printf("\n\nFinal score: %d\n\n", size - 3);

  return 0;
}

/*============================================================*/

void renderGrid(int cells[]) {
  int tracker = 0,
  temp;

  system("@cls||clear");
  printf("\33[?25l");

  // Printing first line
  printf("%c", 201);
  for (int i = 0; i < side_length * 2; i++) printf("%c", 205);
  printf("%c\n", 187);

  // Printing middle lines
  for (int i = 0; i < side_length; i++) {
    printf("%c", 186);
    for (int j = 0; j < side_length; j++) {
      temp = cells[tracker];

      if (temp == 1) printf("%c%c", 219, 219); // Head
      else if (temp == 2) printf("%c%c", 177, 177); // Body
      else if (temp == 3) printf("=%c", 254); // Food
      else if (temp == 4) printf("[]");  // Collide
      else printf("%c%c", 255, 255); // Empty

      tracker++;
    }
    printf("%c\n", 186);
  }

  // Printing last line
  printf("%c", 200);
  for (int i = 0; i < side_length * 2; i++) printf("%c", 205);
  printf("%c", 188);

  return;
}

/*============================================================*/

void updateSnake(int cells[], int snake[], int velocity, int size) {
  int head = snake[0];

  // Updating only if there is non-zero velocity
  if (velocity) {
    cells[snake[size - 1]] = 0;

    // Updating body
    for (int i = size - 1; i > 0; i--) snake[i] = snake[i - 1];

    switch (velocity) {
      case 1:   // Left
        if (head % side_length == 0) head += (side_length - 1);
        else head--;
        break;
      case 2:   // Down
        if (head / side_length == (side_length - 1)) head -= side_length * (side_length - 1);
        else head += side_length;
        break;
      case 3:   // Right
        if (head % side_length == (side_length - 1)) head -= (side_length - 1);
        else head++;
        break;
      default:  // Up
        if (head < side_length) head += side_length * (side_length - 1);
        else head -= side_length;
        break;
    }

    snake[0] = head;
    cells[head] = 1;
    cells[snake[1]] = 2;
  }

  return;
}
