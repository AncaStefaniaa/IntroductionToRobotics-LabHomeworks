#include <LedControl.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

// LCD
const int RS = 9;
const int enable = 3;
const int d4 = 7;
const int d5 = 6;
const int d6 = 5;
const int d7 = 4;
const int LCDnumberOfRows = 2;
const int LCDnumberOfColumns = 16;
LedControl ls = LedControl(12, 11, 10, 1); //data input, clock, load, nr drivers
LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);

// JOYSTICK
const int pinX = A1;
const int pinY = A0;
const int swPin = A3;
const int lowThreshold = 200;
const int highThreshold = 900;
int xValue = 0;
int yValue = 0;
int swValue = 0;
bool swPressed = false;
bool movedX = false;
bool movedY = false;
unsigned long long previousSwitchTime = 0;
unsigned long long currentTime;
// GAME
const int messageSpeed = 5;
int level = 1;
int highScore = 0;
int highScoreStored = 0;
int score = 0;
bool win = false;
bool gameOver = false;
bool gameStart = false;
bool levelStarted = false;
bool barrier = false; //this will be true when the user will reach the minimum score required to pass the level
int barrierValue = 7;
int currentSpeed = 200;
struct Point {
  int row = 0, col = 0;
  Point(int row = 0, int col = 0): row(row), col(col){}
};


// SNAKE
int snakeLength = 0;
int initialSnakeLength = 3;
int snakeDirection = 0;
int gameBoard[8][8] = {};  //snake body segments storage
Point snake[64];
int initialSnakeX = 3;
int initialSnakeY = 3;
int rowHead = 0;
int colHead = 0; 

int currentTranslationDelay = 170;
int initialTranslationDelay = 170;

int currentMillis = 0;
int previousMillis = 0;


// DIRECTION CONSTANTS
const short up = 1;
const short right = 2;
const short down = 3;
const short left = 4;
bool moveLeft = false;
bool moveRight = false;
bool moveUp = false;
bool moveDown = false;

// FOOD
bool eatenFood = 0;
int blinkFoodInterval = 200;
int curFoodBlinkMillis = 0;
int prevFoodBlinkMillis = 0;
Point food;
Point prevFood;

// MENU
int page = 0;
int  selected[4] = {0, 0, 0, 0};
int currentPage = 0;// 0 - startGame, 1 - highScore, 2 - settings, 3 - info
char welcome[] = "Welcome!";
char start[] = "Start";
char settings[] = "Settings";
char highScoreMenu[] = "HighScore";
char info[] = "Info";
char arrow[] = ">";
int arrowPosition = 0;
bool increaseArrowPosition = false;
bool decreaseArrowPosition = false;
bool enterSelection = false;
int previousShowTime = 0;

const PROGMEM bool gameOverMessage[8][90] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

bool Level1[8][8] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
};


bool Level2[8][8] = {
  {0, 0, 0, 1, 1, 0, 0, 0},
  {0, 0, 0, 1, 1, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 1, 1, 0, 0, 0},
  {0, 0, 0, 1, 1, 0, 0, 0}
};

bool Level3[8][8] = {
  {0, 0, 0, 1, 1, 0, 0, 0},
  {0, 0, 0, 1, 1, 0, 0, 0},
  {0, 0, 0, 1, 1, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 1, 1, 0, 0, 0},
  {0, 0, 0, 1, 1, 0, 0, 0},
  {0, 0, 0, 1, 1, 0, 0, 0}
};

bool sadFace[8][8] = {
{0, 0, 0, 0, 0, 0, 0, 0}, // 0
{0, 0, 1, 0, 0, 1, 0, 0}, // 1
{0, 0, 1, 0, 0, 1, 0, 0}, // 2
{0, 0, 0, 0, 0, 0, 0, 0}, // 3
{0, 0, 0, 0, 0, 0, 0, 0}, // 4
{0, 1, 1, 1, 1, 1, 1, 0}, // 5
{1, 0, 0, 0, 0, 0, 0, 1}, // 6
{0, 0, 0, 0, 0, 0, 0, 0}, // 7
};






// MAP FUNCTION
void printMap(bool matrix[8][8]){
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      ls.setLed(0, col, row, matrix[col][row]);
    }
  }
}

void showGameOverMessage(bool matrix[8][8]){
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      ls.setLed(0, col, row, matrix[col][row]);
    }
  }
}


// SNAKE DISPLAY FUNCTIONS
void showSnake(bool matrix[8][8]){
  for(int i = 0; i < snakeLength; i++){
    // this will turn on leds representing the snake
    // initially snakeLength will be 0 so it won't appear
    matrix[snake[i].row][snake[i].col] = true; 
  }
}


// AT GAME START
void initialize(bool matrix[8][8]){
  
 rowHead = initialSnakeX;
 colHead = initialSnakeY;
 // constructing the rest of the body
 for(int i = 0; i < 3; i++){
   snake[i].row = rowHead;
   snake[i].col = colHead - i;
  }
  // set initial snake values
  // the snake will move right, with 3 body segments
  // and speed 100
  snakeLength = 3;
  moveRight = true;
  currentSpeed = 100;
  // this means that snake length isn't bigger than barrier value so game isn't over
  barrier = false;
  // now we put food randomly on map
  food.row = random(8);
  food.col = random(8);
  
}


// FOOD 
void blinkFood(bool matrix[8][8]){
  
  curFoodBlinkMillis = millis();
  if(curFoodBlinkMillis - prevFoodBlinkMillis >= blinkFoodInterval){
    prevFoodBlinkMillis = curFoodBlinkMillis;
    matrix[food.row][food.col] =! matrix[food.row][food.col];
  }
  
}

void eat(bool matrix[8][8]){
  
  int prevPiece = snakeLength - 1;
  int newPiece = snakeLength;
  //  here we will make our snake grow by eating food
  if(prevPiece > 9) return; // if the snake length is bigger than 9 it will not grew up

  // if not, we will turn on a new piece
  snake[newPiece].row = snake[prevPiece].row;
  snake[newPiece].col = snake[prevPiece].col;
  //  here we will turn on the new led on matrix
  matrix[snake[newPiece].row][snake[newPiece].col] = true;
  eatenFood = true;
  snakeLength += 1;
  score +=1;
  
}

// this function will generate random coords for food while game is not over
void generateFood(bool matrix[8][8]){
  
  Point prevFood;
  prevFood.row = food.row;
  prevFood.col = food.col;
  // if food isn't on map then we check if we won
  if(food.row == -1 || food.col == -1)
    if(snakeLength >= 9){
      win = true;
      return;
    } 
  // if not, generate a new piece of food on map   
  do{
    food.col = random(8);
    food.row = random(8);    
  }while(matrix[food.row][food.col] > 0);

  // and turn its led on
  matrix[food.row][food.col] = true;

  // turn off the last food piece on map
  if(eatenFood){
    matrix[prevFood.row][prevFood.col] = false;
    eatenFood = !eatenFood;
  }
  
}


// MOVE SNAKE ON MAP
void moveSnake(bool matrix[8][8], int newRow, int newCol){
  //here we will turn off the snake which represents the snake 
  //from bottom to head
  //frist turn off the bottom led
  matrix[snake[snakeLength - 1].row][snake[snakeLength - 1].col] = false;
  // then one by one the rest of the body parts will inherit bottom state
  for (int i = snakeLength - 1; i >= 1; i--) {
    snake[i] = snake[i - 1]; 
    }
  // check if the snake head reaches the food coords  
  if(newRow == food.row && newCol == food.col){
    eat(matrix);
    currentTranslationDelay -= 5;
    generateFood(matrix);
  }
  else{
    // the food will blink until the snake will reach its coords
    blinkFood(matrix);
  }
  // new we check if the new piece is valid and we turn that led on
  // if it isn't, it means that its already a piece of snake and 
  // we stop the game 
  if(matrix[newRow][newCol] == 0){
    snake[0].row = newRow;
    snake[0].col = newCol;
    matrix[newRow][newCol] = true;
  }
  else{
    if(newRow != food.row && newCol != food.col)
      gameOver = true;
  }
}

// MOVE DIRECTIONS
// these four function ensure that the snake don't go outside our matrix
void moveRightt(bool matrix[8][8]){
  // check if the snake's head go outside our matrix
  // if not then ne will move foward
  if(rowHead < 7) rowHead += 1;
  // otherwise he will be moved to start
  else{
    rowHead = 0;
    if(barrier){
      level++;
      levelStarted = false;
      barrier = false;
    }
  }
  moveSnake(matrix, colHead, rowHead);
}

void moveLeftt(bool matrix[8][8]){
  // check if the snake's head go outside our matrix
  // if not then ne will move foward
  if(rowHead > 0) rowHead -= 1;
  // otherwise he will be moved to start
  else{
    rowHead = 7;
    if(barrier){
      level++;
      levelStarted = false;
      barrier = false;
    }
  }
  moveSnake(matrix, colHead, rowHead);
}

void moveUpp(bool matrix[8][8]){
  // check if the snake's head go outside our matrix
  // if not then ne will move foward
  if(colHead < 7) colHead += 1;
  // otherwise he will be moved to start
  else{
    colHead = 0;
    if(barrier){
      level++;
      levelStarted = false;
      barrier = false;
    }
  }
  moveSnake(matrix, colHead, rowHead);
}

void moveDownn(bool matrix[8][8]){
  // check if the snake's head go outside our matrix
  // if not then ne will move forward
  if(colHead > 0) colHead -= 1;
  // otherwise he will be moved to start
  else{
    colHead = 7;
    if(barrier){
      level++;
      levelStarted = false;
      barrier = false;
    }
  }
  moveSnake(matrix, colHead, rowHead);
}



// MOVE HEAD
void moveHead(bool matrix[8][8]){
  // read joystick values for x and y
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
  
  if(movedX == false && movedY == false){
    if(xValue > highThreshold && (moveUp || moveDown)){
      movedX = true;
      moveLeft = false;
      moveRight = true;
      moveUp = false;
      moveDown = false;
    }
    if(xValue < lowThreshold && (moveUp || moveDown)){
      movedX = true;
      moveLeft = true;
      moveRight = false;
      moveUp = false;
      moveDown = false;
    }
  }
  if(movedX == false && movedY == false){
    if (yValue > highThreshold && (moveRight || moveLeft)) {
      movedY = true;
      moveLeft = false;
      moveRight = false;
      moveUp = false;
      moveDown = true;
    }
    
    if (yValue < lowThreshold && (moveRight || moveLeft)) {
      movedY = true;
      moveLeft = false;
      moveRight = false;
      moveUp = true;
      moveDown = false;
    }
  }

  if(yValue >= lowThreshold && yValue <= highThreshold && movedY) movedY = false;
  if(xValue >= lowThreshold && xValue <= highThreshold && movedX) movedX = false;



  // ne asiguram ca se misca incontinuu pe o directie pana la schimbare
  if(moveLeft){
    currentMillis = millis();
    if (currentMillis - previousMillis >= currentTranslationDelay) {
      moveLeftt(matrix);
      previousMillis = currentMillis;
    }
  }

  if(moveRight){
    currentMillis = millis();
    if (currentMillis - previousMillis >= currentTranslationDelay) {
      moveRightt(matrix);
      previousMillis = currentMillis;
    }
  }

  if(moveDown){
    currentMillis = millis();
    if (currentMillis - previousMillis >= currentTranslationDelay) {
      moveDownn(matrix);
      previousMillis = currentMillis;
    }
  }
  
  if(moveUp){
    currentMillis = millis();
    if (currentMillis - previousMillis >= currentTranslationDelay) {
      moveUpp(matrix);
      previousMillis = currentMillis;
    }
  }
}

void handleGameState(){
  
  if(gameOver || win){
    if(gameOver) showGameOverMessage(sadFace);
    else if(win) 
    // re-init the game
    win = false;
    gameOver = false;
    snake[0].row = random(8);
    snake[0].col = random(8);
    food.row = -1;
    food.col = -1;
    snakeLength = initialSnakeLength;
    snakeDirection = 0;
    ls.clearDisplay(0);
  }
  
}

// LEVELS
void level_1(){
  // here we will display different a different matrix for each level
  // with different difficulties
  if(!levelStarted){
    initialize(Level1);
    levelStarted = true;
  }
  if(!gameOver){
    moveHead(Level1);
    printMap(Level1);  //displays the map for level 1
    //check if the snake reaches the
    if (snakeLength >= barrierValue){
      
      barrier = true;
      
      /*for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      ls.setLed(0, col, row, sadFace[col][row]);
    }
  }*/
    }
  }else{
   showGameOverMessage(sadFace);
  }
}


void level_2(){
  // here we will display different a different matrix for each level
  // with different difficulties
  if(!levelStarted){
    initialize(Level2);
    levelStarted = true;
  }
  if(!gameOver){
    moveHead(Level2);
    printMap(Level2);  //displays the map for level 1
    if (snakeLength >= barrierValue){
      barrier = true;
      /*
      for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      ls.setLed(0, col, row, sadFace[col][row]);
    }
  }*/
    }
  }else{
    showGameOverMessage(sadFace);
  }
}

void level_3(){
  // here we will display different a different matrix for each level
  // with different difficulties
  if(!levelStarted){
    initialize(Level2);
    levelStarted = true;
  }
  if(!gameOver){
    moveHead(Level3);
    printMap(Level3);  //displays the map for level 1
    if (snakeLength >= barrierValue){
      barrier = true;
      for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      ls.setLed(0, col, row, sadFace[col][row]);
    }
  }
    }
  }else{
    showGameOverMessage(sadFace);
  }
}

// LCD AND GAME CONTROL FUNCTIONS
void startGame(){
 if(swPressed){
    gameStart = false;
    gameOver = true;
  }
  if(!gameOver){
    switch(level){
      case 1:
      level_1();
      break;
      case 2:
      level_2();
      break;
      case 3:
      level_3();
      break;
     }
    gameStart = true;
  }
  else if(gameOver){
    showGameOverMessage(sadFace);
    gameStart = false;
  }
  lcd.clear();
  lcd.print("Score:");
  lcd.setCursor(7, 0);
  lcd.print(score);
}

// LCD

void switchState(){
  
  int swValue = digitalRead(swPin);
  if(!swValue && previousSwitchTime + 150 < currentTime){
    swPressed = 1;
    previousSwitchTime = currentTime;
    Serial.println("pushed");
  }
  else 
    swPressed = false;
  
}

void showInfo(){
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Autor - Anca");
  
}
void showHighScore(){
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Highscore:");
  lcd.print(highScore);

}

void showSettings(){
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Level:");
  lcd.print(level);
  lcd.setCursor(0, 1);
  lcd.print("Speed:");
  lcd.print(currentSpeed);
  
}
void showPage(){
  
  switch(currentPage){
    case(0):
      startGame();
      break;
    case(1):
      showHighScore();
      break;
    case(2):
      showSettings();
      break;
    case(3):
      showInfo();
      break;    
  }
  
}

void swMenu(){
  
  xValue = analogRead(pinX);
  if(xValue > highThreshold) xValue = 1;
  else if(xValue < lowThreshold) xValue = -1;
  xValue = analogRead(pinY);
  if(yValue > highThreshold) yValue = 1;
  else if(yValue < lowThreshold) yValue = -1;
  
}

void showMenu(){
  
  if(previousShowTime + 50 >= currentTime) return;
  lcd.clear();
  lcd.setCursor(0, 0);
  char menu[3][30] = {"StartGame", "Score", "Settings"}; ;
  int first, last, currentLine = 0;
   if(selected[currentPage] >= LCDnumberOfRows - 1){
     first = selected[currentPage] - LCDnumberOfRows + 1;
     last = selected[currentPage];
   }
   else{
    first = 0;
    last = LCDnumberOfRows - 1;
   }

   for (int i = first; i <= last; ++i){
       lcd.setCursor(0, currentLine++);
       if (selected[currentPage] == i)lcd.print(">");
       else lcd.print(" ");
       lcd.print(menu[i]);
       lcd.print(" ");
   }

    previousShowTime = currentTime;
    
}
void setup() {
  // put your setup code here, to run once:
 // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);
  pinMode(swPin, INPUT_PULLUP);
  ls.shutdown(0, false); //0 represents our first driver turned on always
  ls.setIntensity(0, 2); //intensity is from 0 to 15
  ls.clearDisplay(0);  //clears the display on the first driver
  /*
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      ls.setLed(0, col, row, true);
      delay(25);
    }
  }
  */
  lcd.begin(16, 2);
  highScoreStored = EEPROM.read(0);
  if(highScoreStored == 255){
    EEPROM.write(0, 0);
    highScoreStored = EEPROM.read(0);
  }
  Point food(3, 6); //set initial food location
  
}

void loop() {
  currentTime = millis();
  // put your main code here, to run repeatedly:
 
 // switchState();
  //showMenu();
  startGame();
  
}
