#include "LedControl.h"//Include library to control  //the MAX7219 matrix.

#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 8, en = 9, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

LedControl myMatrix = LedControl(12, 11, 10, 1);//Creates a matrix //instance.
/* DIN  -> Arduino Digital 12
   CS   -> Arduino Digital 11
   CLK  -> Arduino Digital 10
   The forth digit indicates that I'm using only 1 MAX7219 module.
 */  

// 1.Global variables ---
int column = 1, row = random(8)+1;     //Decides which point the ball must //start.
int directionX = 1, directionY = 1;    //Certificates that the ball go first //from right to left.
int paddle1 = 5, paddle1Val;           //Pin and potentiometer value.
int velocidade = 100;                  //Ball's initial speed.
int counter = 0, mult = 10;
char score[4];             // Variable to convert int counter in char.
// end 1. ---

//2.Initial settings ----

// Variables used for rolling score, both start on 0.
int index = 0;  // Current char to be shown.
int shift = 0; // Quantity of shifting columns.

// Array to store bit map to be shown.
int bitsMap[] = {0,0,0,0,0,0,0,0};
// Vector with power of 2 to calculate what to show.
const int twoPowers[] = {1,2,4,8,16,32,64,128};

int charachter;   
#define displayspeed 25 //Score exhibition speed, less is faster.
//End 2. ----

//How numbers must appear on display.
const int zero[]={B00111100,
                  B01000010,
                  B01000010,
                  B01000010,
                  B01000010,
                  B01000010,
                  B00111100,
                  B00000000};
const int one[]={B00001000,
                 B00011000,
                 B00101000,
                 B00001000,
                 B00001000,
                 B00001000,
                 B00111110,
                 B00000000};
const int two[]={B00011000,
            B00100100,
            B00100100,
            B00001000,
            B00010000,
            B00100000,
            B00111100,
            B00000000};
const int three[]={B00111100,
              B01000010,
              B00000010,
              B00111100,
              B00000010,
              B01000010,
              B00111100,
              B00000000};
const int four[]={B00000100,
             B00001100,
             B00010100,
             B00100100,
             B01111111,
             B00000100,
             B00000100,
             B00000000};
const int five[]={B01111110,
             B01000000,
             B01111000,
             B00000100,
             B00000010,
             B00000010,
             B01111100,
             B00000000};
const int six[]={B00111100,
                 B01000000,
                 B01000000,
                 B01111100,
                 B01000010,
                 B01000010,
                 B00111100,
                 B00000000};
const int seven[]={B01111100,
                   B00000100,
                   B00000100,
                   B00011111,
                   B00000100,
                   B00000100,
                   B00000100,
                   B00000000};
const int eight[]={B00111100,
                   B01000010,
                   B01000010,
                   B00111100,
                   B01000010,
                   B01000010,
                   B00111100,
                   B00000000};
const int nine[]={B00111100,
                  B01000010,
                  B01000010,
                  B00111110,
                  B00000010,
                  B00000010,
                  B01111100,
                  B00000000};

void setup()
{
    myMatrix.shutdown(0, false);                 //Habilitate display.
    myMatrix.setIntensity(0, 8);                 //Define medium brightness.
    myMatrix.clearDisplay(0);                    //Clear display.
    randomSeed(analogRead(0));                   //Creates a random seed.
    Serial.begin(9600);   
    lcd.begin(16, 2);    
    lcd.print("START THE GAME!");                   //To appear score on serial //screen.
} 

void loop()
{   
    lcd.setCursor(0, 1);
    paddle1Val = analogRead(paddle1);
    paddle1Val = map(paddle1Val, 200, 1024, 1, 6);
    column += directionX;
    row += directionY;

    if(column == 6 && directionX == 1 && (paddle1Val == row || paddle1Val+1 == row || paddle1Val+2 == row)) directionX = -1;

    if(column == 0 && directionX == -1) directionX = 1;

    if(row == 7 && directionY == 1 ) directionY = -1;

    if(row == 0 && directionY == -1) directionY = 1;

    if(column == 7){     
      oops();
    }
    myMatrix.clearDisplay(0);                    //Clears screens for the //next animation frame.
    myMatrix.setLed(0, column, row, HIGH);
    myMatrix.setLed(0, 7, paddle1Val, HIGH);
    myMatrix.setLed(0, 7, paddle1Val+1, HIGH);
    myMatrix.setLed(0, 7, paddle1Val+2, HIGH);

    if(!(counter % mult)) {velocidade -= 5; mult*mult;}
    delay(velocidade);
    counter++; 
}

void oops() //Game over function.
{      
    delay(1000);
    if(counter<100){//If score is lower than 100.
      for(int y=0; y<40; y++)
       {   
          loadMatrix2();
          drawScreen(bitsMap);
          //What the serial screen must show.
          lcd.println("Game Over:");
          lcd.print(counter);
          lcd.println("\n");
          lcd.println("Score:");
          lcd.print(score);
          lcd.println("\n");
       }
      }else{   //If score is higher than 100.
       for(int y=0; y<40; y++)
       {
          loadMatrix();
          drawScreen(bitsMap);
          //What the serial screen must show.
          lcd.println("Game Over:");
          lcd.print(counter);
          lcd.println("\n");
          lcd.println("Score:");
          lcd.print(score);
          lcd.println("\n");
       }         
       }
       delay(1000);  
  
  counter = 0;  //Reinitiate all values.
  velocidade = 300;
  column = 1;
  row = random(8)+1;
} 

void loadMatrix(){ //Load matrix to show 3 digits.
  itoa(counter,score,10); //Functions that convert int in char strings of 10 //base, to be shown at display.
  getChar(score[index]); // Reads current phrase char.
  int currentChar = charachter;
  getChar(score[index+1]); // Reads next char.
  int nextChar = charachter; 
  executeMatrix(currentChar, nextChar);
}

void loadMatrix2(){//Load matrix to show 1 or 2 digits.
  itoa(counter,score,10);  
  getChar(score[0]); // Reads current phrase char..
  int currentChar = charachter;
  getChar(score[1]); // Reads next char.
  int nextChar = charachter; 
  executeMatrix(currentChar, nextChar);
}

// Creates the matrix's bit map.
void executeMatrix(int currentBuffer[], int nextBuffer[]) { 
  for(int r=0; r < 8; r++){ // Increments r to r from 0 to 7 (row - line).
    bitsMap[r] = 0;  // Reset line.
    for(int c=0; c < 8; c++){ // Increments c to c from 0 to 7 (column - //coluna).
     bitsMap[r] = bitsMap[r] + ((twoPowers[c] & (currentBuffer[r] << shift))); // Loads current char compensated by shifting.
     bitsMap[r] = bitsMap[r] + (twoPowers[c] & (nextBuffer[r] >> (8 - shift) ));  // Loads the next char compensated by shifting.
    }
  }
  shift++; // Increments the shift variable responsible by one line //shifting.
  
  if(shift==8){
    shift = 0; 
    index++; 
    //if(index == sizeof(texto)-2){
    if(index == sizeof(score)-2){
      index=0;
     }
  }            // If shift is equal to 8, load the next char pair. //(currentChar and nextCahr).
}

void  drawScreen(int buffer[]) {
  for (int v = 0; v < displayspeed; v++) {
      for (int i = 0; i < 8; i++) { // linhas
        for (int j = 0; j < 8; j++) { // colunas
          myMatrix.setLed(0, i, j, bitRead(buffer[i], 7 - j));     
 //using bitRead()
        }
    }
  }
}

void getChar(char msgChar) {
 switch(msgChar){
  case '0': charachter = zero; break;
  case '1': charachter = one; break;
  case '2': charachter = two; break;
  case '3': charachter = three; break;
  case '4': charachter = four; break;
  case '5': charachter = five; break;
  case '6': charachter = six; break;
  case '7': charachter = seven; break;
  case '8': charachter = eight; break;
  case '9': charachter = nine; break; 
  }
}