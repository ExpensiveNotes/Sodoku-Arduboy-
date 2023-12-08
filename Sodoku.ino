/*
  Arbuboy Game SOS by John Wegner
  aka Expensive Notes
  version 3 with Font4x6 by filmote on the Arduboy Forums

  buttons
  B - Mode choice
  A - ENTER Select to choose current number or place current number on the grid
  D - Arrows/Cursor movement. Alter values when selecting them.

  modes
  0 New Game
  1 choose current number/flash it?
  2 position cursor and place new value into grid
  3 zoom mode into one block because of the small screen size!

*/

#include <Arduboy2.h>

Arduboy2 arduboy;
#include "Font4x6.h"

Font4x6 font4x6 = Font4x6();


/*
   Modes
   0 New Game
  1 choose current number/flash it?
  2 position cursor and place new value into grid
  3 zoom mode into one block because of the small screen size!
*/
byte mode = 0;

byte grid[9][9]; //For display
byte gridTemplate[9][9] = {{1, 2, 3, 4, 5, 6, 7, 8, 9}, {4, 5, 6, 7, 8, 9, 1, 2, 3}, {7, 8, 9, 1, 2, 3, 4, 5, 6}, {2, 3, 4, 5, 6, 7, 8, 9, 1}, {5, 6, 7, 8, 9, 1, 2, 3, 4}, {8, 9, 1, 2, 3, 4, 5, 6, 7}, {3, 4, 5, 6, 7, 8, 9, 1, 2}, {6, 7, 8, 9, 1, 2, 3, 4, 5}, {9, 1, 2, 3, 4, 5, 6, 7, 8}};
byte gridSolution[9][9];
byte currentNumberInBlockGrid[3][3];
char message[20];

int cursorX = 0, cursorY = 0;
byte flashTimer = 30;
byte currentNumber = 5;
bool gridFilled = false;
byte level = 2; //How many numbers are revealed

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(20);
  arduboy.initRandomSeed(); // <-- seed the random number generator
  newGame();
}

void loop() {
  if (!arduboy.nextFrame()) {
    return;
  }
  checkButtons();
  flashIt();
  checkForWin();
  checkForDuplicates();
  checkGridForCurrentNumber();
  displayStuff();
}

void newGame() {
  makeGrid();
  mode = 0;
  strcpy(message, "");
}

void flashIt() {
  flashTimer--;
  if (flashTimer == 0) flashTimer = 30;
}

void checkButtons() {
  arduboy.pollButtons();
  switch (mode) {
    case 0:
      // New Game
      chooseLevel();
      break;
    case 1:
      // choose current number/flash it?
      choosecurrentNumber();
      break;
    case 2:
      // position cursor and place new value into grid
      moveCursorAndPlaceNumber();
      break;
    case 3:
      // position cursor and place new value into grid
      moveCursorAndPlaceNumber();
      break;
    default:
      break;
  }
}

void chooseLevel() {
  if (arduboy.justPressed(UP_BUTTON)) level--;
  if (arduboy.justPressed(DOWN_BUTTON)) level++;
  if (arduboy.justPressed(LEFT_BUTTON)) level = level - 3;
  if (arduboy.justPressed(RIGHT_BUTTON)) level = level + 3;
  if (level < 1) level = 1;
  if (level > 5) level = 5;
  if (arduboy.justPressed(A_BUTTON)) {
    mode = 1;
    chooseGrid();
  }
}

void choosecurrentNumber() {
  if (arduboy.justPressed(UP_BUTTON)) currentNumber--;
  if (arduboy.justPressed(DOWN_BUTTON)) currentNumber++;
  if (arduboy.justPressed(LEFT_BUTTON)) currentNumber = currentNumber - 3;
  if (arduboy.justPressed(RIGHT_BUTTON)) currentNumber = currentNumber + 3;
  if (currentNumber < 1) currentNumber = 1;
  if (currentNumber > 9) currentNumber = 9;
  //confirm number and change to cursor mode
  if (arduboy.justPressed(A_BUTTON)) mode = 2;
  if (arduboy.justPressed(B_BUTTON)) mode = 2;
  //reset game
  if (arduboy.justPressed(A_BUTTON) && arduboy.justPressed(B_BUTTON)) mode = 0;
}

void moveCursorAndPlaceNumber() {
  if (arduboy.justPressed(UP_BUTTON)) {
    cursorY--;
    strcpy(message, "");
  }
  if (arduboy.justPressed(DOWN_BUTTON)) {
    cursorY++;
    strcpy(message, "");
  }
  if (arduboy.justPressed(LEFT_BUTTON)) {
    cursorX--;
    strcpy(message, "");
  }
  if (arduboy.justPressed(RIGHT_BUTTON)) {
    cursorX++;
    strcpy(message, "");
  }
  if (cursorY < 0) cursorY = 8;
  if (cursorY > 8) cursorY = 0;
  if (cursorX < 0) cursorX = 8;
  if (cursorX > 8) cursorX = 0;
  if (arduboy.justPressed(A_BUTTON)) {
    if (grid[cursorX][cursorY] == currentNumber) grid[cursorX][cursorY] = 0;
    else grid[cursorX][cursorY] = currentNumber;
  }
  //Change to "choose number" mode?
  if (arduboy.justPressed(B_BUTTON)) mode++;
  if (mode > 3) mode = 1;
  //reset game
  if (arduboy.justPressed(A_BUTTON) && arduboy.justPressed(B_BUTTON)) mode = 0;
}

void chooseGrid() {
  for (byte x = 0; x < 9; x++) {
    for (byte y = 0; y < 9; y++) {
      if (random(level + 1) == 0) grid[x][y] = gridSolution[x][y];
      else grid[x][y] = 0;
    }
  }
  gridFilled = false;
}

void displayStuff() {
  arduboy.clear();
  arduboy.setCursor(72, 0);
  arduboy.print("-=Sodoku=-");
  arduboy.setCursor(74, 15);
  arduboy.print(message);
  arduboy.setCursor(74, 30);
  switch (mode) {
    case 0:
      // New Game
      arduboy.print("Level?");
      arduboy.setCursor(74, 50);
      arduboy.print(level);
      break;
    case 1:
      // choose current number/flash it?
      arduboy.print("Number?");
      showCurrentNumber();
      showGridWithNumberFlashing();
      showcurrentNumberInBlockGrid();
      break;
    case 2:
      // position cursor and/or place new value into grid
      arduboy.print("Position?");
      showCurrentNumber();
      showGridWithCursorFlashing();
      showcurrentNumberInBlockGrid();
      break;
    case 3:
      // position cursor and/or place new value into grid
      arduboy.print("Position?");
      showCurrentNumber();
      showZoomGridWithCursor();
      showcurrentNumberInBlockGrid();
      showCurrentCursorBlock();
      break;
    default:
      break;
  }
  arduboy.display();
}

void showCurrentNumber() {
  arduboy.setTextSize(2);
  arduboy.setCursor(80, 42);
  arduboy.print(currentNumber);
  arduboy.setTextSize(1);
}

void showcurrentNumberInBlockGrid() {
  for (byte x = 0; x < 3; x++) {
    for (byte y = 0; y < 3; y++) {
      if (currentNumberInBlockGrid[x][y] == 1) {
        font4x6.setCursor(100 + x * 6, 40 + y * 6);
        font4x6.print("o");
      }
    }
  }
}

//show which block the cursor is in
void showCurrentCursorBlock() {
  byte bx = cursorX / 3;
  byte by = cursorY / 3;
  arduboy.fillCircle(101 + bx * 6, 44 + by * 6, 1);
}

void showGridWithNumberFlashing() {
  for (byte x = 0; x < 9; x++) {
    for (byte y = 0; y < 9; y++) {
      if (grid[x][y] == currentNumber) {
        if ( flashTimer < 15) {
          font4x6.setCursor(x * 8, y * 7);
          printGridCell(grid[x][y]);
        }
      } else {
        font4x6.setCursor(x * 8, y * 7);
        printGridCell(grid[x][y]);
      }
    }
  }
}

void showGridWithCursorFlashing() {
  for (byte x = 0; x < 9; x++) {
    for (byte y = 0; y < 9; y++) {
      if (cursorX == x && cursorY == y) {
        if ( flashTimer < 15) {
          font4x6.setCursor(x * 8, y * 7);
          printGridCell(grid[x][y]);
        }
      } else {
        font4x6.setCursor(x * 8, y * 7);
        printGridCell(grid[x][y]);
      }
    }
  }
}

void showZoomGridWithCursor() {
  byte bx = cursorX / 3;
  byte by = cursorY / 3;
  for (byte x = 0; x < 3; x++) {
    for (byte y = 0; y < 3; y++) {
      if ( cursorX == bx * 3 + x && cursorY == by * 3 + y) {
        if ( flashTimer < 15) {
          arduboy.setCursor(15 + x * 16, 10 + y * 14);
          printZoomGridCell(grid[bx * 3 + x][by * 3 + y]);
        }
      } else {
        arduboy.setCursor(15 + x * 16, 10 + y * 14);
        printZoomGridCell(grid[bx * 3 + x][by * 3 + y]);
      }
    }
  }

  //columns - check if current number is in this column
  for (byte x = 0; x < 3; x++) {
    for (byte y = 0; y < 9; y++) {
      if (grid[bx * 3 + x][y] == currentNumber) {
        arduboy.setCursor(15 + x * 16, 0);
        arduboy.print(currentNumber);
      }
    }
  }
  //rows -check if current number is in this row
  for (byte y = 0; y < 3; y++) {
    for (byte x = 0; x < 9; x++) {
      if (grid[x][by * 3 + y] == currentNumber) {
        arduboy.setCursor(0, 10 + y * 14);
        arduboy.print(currentNumber);
      }
    }
  }
  //which block is the cursor in
  arduboy.setCursor(0, 55);
  arduboy.print("Block ");
  arduboy.print(bx + 1);
  arduboy.print(":");
  arduboy.print(by + 1);
}

void printGridCell(byte num) {
  if (num == 0) font4x6.print(".");
  else font4x6.print(num);
}

void printZoomGridCell(byte num) {
  if (num == 0) arduboy.print(".");
  else arduboy.print(num);
}

void checkGridForCurrentNumber() {
  for (byte bx = 0; bx < 3; bx++) {
    for (byte by = 0; by < 3; by++) {
      currentNumberInBlockGrid[bx][by] = 0;
      for (byte x = 0; x < 3; x++) {
        for (byte y = 0; y < 3; y++) {
          if (grid[bx * 3 + x][by * 3 + y] == currentNumber) currentNumberInBlockGrid[bx][by] = 1;
        }
      }
    }
  }
}

void makeGrid() {
  //copy from template
  for (byte x = 0; x < 9; x++) {
    for (byte y = 0; y < 9; y++) {
      gridSolution[x][y] = gridTemplate[x][y];
    }
  }
  shuffleNumbers();
}

void shuffleNumbers() {
  //shuffles 2 columns or 2 rows. But they have to  be in the same block
  for (byte i = 0 ; i < 20; i++) {
    bool vertical = random(2); //Vertical or Horizontal
    byte randomNumb = random(3); //Choose random block (There are 9 blocks 3x3)
    if (vertical) shuffleColumns(randomNumb);
    else shuffleRows(randomNumb);
  }
}

void shuffleColumns(byte block) {
  byte col1 = block * 3 + random(3);
  byte col2 = block * 3 + random(3);
  byte tmp;
  for (byte y = 0; y < 9; y++) {
    tmp = gridSolution[col1][y];
    gridSolution[col1][y] = gridSolution[col2][y];
    gridSolution[col2][y] = tmp;
  }
}

void shuffleRows(byte block) {
  byte row1 = block * 3 + random(3);
  byte row2 = block * 3 + random(3);
  byte tmp;
  for (byte x = 0; x < 9; x++) {
    tmp = gridSolution[x][row1];
    gridSolution[x][row1] = gridSolution[x][row2];
    gridSolution[x][row2] = tmp;
  }
}

void checkForWin() {
  //rows
  for (byte y = 0; y < 9; y++) {
    byte total = 0;
    for (byte x = 0; x < 9; x++) {
      total = total + grid[x][y];
    }
    if (total != 45) return;
  }

  //columns
  for (byte x = 0; x < 9; x++) {
    byte total = 0;
    for (byte y = 0; y < 9; y++) {
      total = total + grid[x][y];
    }
    if (total != 45)  return;
  }

  //blocks
  for (byte bx = 0; bx < 3; bx++) {
    for (byte by = 0; by < 3; by++) {
      byte total = 0;
      for (byte x = 0; x < 3; x++) {
        for (byte y = 0; y < 3; y++) {
          total = total + grid[bx * 3 + x][by * 3 + y];
        }
      }
      if (total != 45)  return;
    }
  }
  strcpy(message, "Yay! Done!");
}

void checkForDuplicates() {
  bool foundNumber[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

  //blocks
  for (byte bx = 0; bx < 3; bx++) {
    for (byte by = 0; by < 3; by++) {
      //each block
      //reset found numbers
      for (byte i = 0; i < 9; i++) foundNumber[i] = false;
      for (byte x = 0; x < 3; x++) {
        for (byte y = 0; y < 3; y++) {
          if (grid[bx * 3 + x][by * 3 + y] != 0 && foundNumber[grid[bx * 3 + x][by * 3 + y] - 1]) {
            //duplicate so flag it
            char tmp [5];
            strcpy(message, "Du B: ");
            itoa(bx + 1, tmp, 10);
            strcat(message, tmp);
            itoa(by + 1, tmp, 10);
            strcat(message, tmp);
            //number found to be a duplicate
            itoa(grid[bx * 3 + x][by * 3 + y], tmp, 10);
            strcat(message, tmp);
            return;
          }
          //set found a number
          foundNumber[grid[bx * 3 + x][by * 3 + y] - 1] = true;
        }
      }
    }
  }

  //rows
  for (byte y = 0; y < 9; y++) {
    //reset found numbers for each row
    for (byte x = 0; x < 9; x++) foundNumber[x] = false;
    //get numbers in row and see if already found
    for (byte x = 0; x < 9; x++) {
      if (grid[x][y] != 0 && foundNumber[grid[x][y] - 1]) {
        //duplicate so flag it
        char tmp [5];
        strcpy(message, "Dup R: ");
        itoa(y + 1, tmp, 10);
        strcat(message, tmp);
        return;
      }
      //set found a number
      foundNumber[grid[x][y] - 1] = true;
    }
  }

  //columns
  for (byte x = 0; x < 9; x++) {
    //reset found numbers for each column
    for (byte y = 0; y < 9; y++) foundNumber[y] = false;
    //get numbers in column and see if already found
    for (byte y = 0; y < 9; y++) {
      if (grid[x][y] != 0 && foundNumber[grid[x][y] - 1]) {
        //duplicate so flag it
        char tmp [5];
        strcpy(message, "Dupl C: ");
        itoa(x + 1, tmp, 10);
        strcat(message, tmp);
        return;
      }
      //set found a number
      foundNumber[grid[x][y] - 1] = true;
    }
  }
}
