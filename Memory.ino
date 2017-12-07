#include <LedControl.h> //  Library for Matrix manipulation
#include <time.h>
#include <stdlib.h> // 2 Libraries for Randomizing
#include <LiquidCrystal.h> // Library for the LCD display
const int RS = 9, EN = 8, D4 = 5, D5 = 4, D6 = 3, D7 = 2, V0 = 6; // Setting pins for the LCD
LiquidCrystal LCD(RS, EN, D4, D5, D6, D7);  // Initializing the LCD
LedControl lc=LedControl(12,11,10,1); // Initializing the Matrix

bool startMatrix[8][8]; // The matrix that is created random
bool userMatrix[8][8];  // The matrix created by the User
bool won = false; // Variable that shows weather the User won or not
int numberComplexity = 5;
int timeComplexity = 5000;
double indexTime = 500; // index by which the time is decreased every step
bool isTime; // variabile that shows weather the user selected a time (true) or number (complexity)
int lives = 5;
int score = 0;

// Variables for the Joystick
const int joyX = 0;
const int joyY = 1;
const int button = 7;
int valueX = 0;
int valueY = 0;
// Current user Position
int currentX;
int currentY;

void setup() {
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0,false);// turn off power saving, enables display
  lc.setIntensity(0,8);// sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen
  pinMode(button, INPUT); // initializes the joystick Buttion
  digitalWrite(button,HIGH);
  Serial.begin(9600);
  srand(time(NULL)); // initializes the randomizer
  analogWrite(V0, 100); 
  LCD.begin(16,2);
  LCD.print("   Welcome to   ");
  LCD.setCursor(0,1);
  LCD.print(" THE MEMORY GAME");
  delay(5000); // Welcome Screen
  LCD.clear();
}

void LCDUpdate()
{
  LCD.clear();
  LCD.print("SCORE ");
  LCD.print(score);
  LCD.setCursor(0,1);
  LCD.print("LIVES ");
  LCD.print(lives);
}

void emptyMatrix(bool matrix[8][8])
{
  for (int i = 0; i<8; i++)
    for (int j = 0; j<8; j++)
      matrix[i][j] = false;
}

void createRandom(bool matrix[8][8], int complexity)
{
  while (complexity > 0)
  {
    int x, y;
    x = rand() % 8;
    y = rand() % 8;
    if (!matrix[x][y])
    {
      matrix[x][y] = true;
      complexity--;      
    }
  }
}

void printMatrix(bool matrix[8][8])
{
  for (int i = 0; i<8; i++)
    for (int j = 0; j<8; j++)
        lc.setLed(0,i,j,matrix[i][j]);
}

// maps the value of the joystick to an integer
int mapping(int data) 
{
  if (data > 600)
    return 0;
  if (data < 400)
    return 2;
  return 1;
}

// moves the cursor on the matrix
void move()
{
  delay(100); // Delay for the User to see the cursor position
  printMatrix(userMatrix);
  lc.setLed(0,currentX,currentY,false);
  valueX = analogRead(joyX);
  valueY = analogRead(joyY);
  int x = mapping(valueY);
  int y = mapping(valueX);
  if (x == 0)
    currentX++;
  if (y == 0)
    currentY--;
  if (x == 2)
    currentX--;
  if (y == 2)
    currentY++;
  if (currentX == 8)
    currentX = 0;
  if (currentY == 8)
    currentY = 0;
  if (currentX == -1)
    currentX = 7;
  if (currentY == -1)
    currentY = 7;
  lc.setLed(0,currentX,currentY,true);
}

bool makeAMove()
{
  currentX = 0;
  currentY = 0;
  while (digitalRead(button) == 1)
    move();
  while(digitalRead(button) == 0)
    delay(1); // waits for the press of the button
  if (startMatrix[currentX][currentY])
  {
    userMatrix[currentX][currentY] = true;
    return true;
  }
  return false;
}

void win()
{
  won = true;
  score++;
  if (isTime)
    indexTime = indexTime * 0.9;
  else
    if (numberComplexity != 64)
      numberComplexity++;
  timeComplexity -= indexTime;
  lives++;
  for (int row=0; row<8; row++)
  {
    for (int col=0; col<8; col++)
    {
      lc.setLed(0,col,row,true); // turns on LED at col, row
      delay(25); // Win screen
    }
  }
 
  for (int row=0; row<8; row++)
  {
    for (int col=0; col<8; col++)
    {
      lc.setLed(0,col,row,false); // turns off LED at col, row
      delay(25); // Win screen
    }
  }
}

void lose()
{
  emptyMatrix(startMatrix);
  printMatrix(startMatrix);
  LCD.clear();
  LCD.print("Final Score: ");
  LCD.print(score);
  delay(3000); //Gives the user time to read
  LCD.clear();
  LCD.print("  Want to play  ");
  LCD.setCursor(0,1);
  LCD.print("NO   again?  YES");
  int choice = analogRead(joyY);
  while(choice > 400 && choice < 600)
    choice = analogRead(joyY);
  if (choice <= 400)
  {
    LCD.clear();
    LCD.print("    GOODBYE     "); 
    delay(100000); // end of game
  }
  else
  {
    lives = 5;
    score = 0;
    won = false;
  }
}

void setDifficulty()
{
  LCD.clear();
  LCD.print("Select Dificulty");
  LCD.setCursor(0,1);
  LCD.print("Time      Number");
  delay(500); // gives the user time to read
  int choice = analogRead(joyY);
  while(choice > 400 && choice < 600)
    choice = analogRead(joyY);
  if (choice <= 400)
  {
    LCD.clear();
    LCD.print("Time Difficulty");
    LCD.setCursor(0,1);
    LCD.print("    selected");
    isTime = true;
    numberComplexity = 5;
    indexTime = 500;
    delay(500);// gives the user time to read
  }
  else
  {
    LCD.clear();
    LCD.print("Number Difficulty");
    LCD.setCursor(0,1);
    LCD.print("    selected");
    isTime = false;
    numberComplexity = 5;
    indexTime = 0;
    delay(500); // gives the user time to read
  }
}

// makes a level
void level()
{
  LCDUpdate();
  emptyMatrix(startMatrix);
  createRandom(startMatrix, numberComplexity);
  printMatrix(startMatrix);
  delay(timeComplexity); // time to remember the configuration
  emptyMatrix(userMatrix);
  int i;
  int pointsGuessed = 0;
  int currentLevelLives = lives;
  for (i = 0; i<numberComplexity+currentLevelLives && lives > 0; i++)
  {
    Serial.println(i);
    Serial.println(timeComplexity);
    LCDUpdate();
    if (makeAMove())
      pointsGuessed++;
    else
      lives--;
    if (pointsGuessed == numberComplexity)
      break;
  }
  if (pointsGuessed == numberComplexity)
    win();
  else
    lose();
}


void loop() 
{
  if (!won)
    setDifficulty();
  level();
}
