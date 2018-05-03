/**
   The purpose of this code is to control the duty cycle
   of the entire device by manipulating the duty cycle
   of the RF transmitter. CURRENTLY ASSUMING 240x320px for display
 **/

#include <UTFT.h>
#include <URTouch.h>
#include <UTFT_tinyFAT.h>
#include <tinyFAT.h>
//#include <font_Arial.h>

#define TOUCH_ORIENTATION  PORTRAIT

//Transmitter Setup
//int pinNum = 10;                                   //UI Tx -> Transmitter Rx serial wire if tx unavailable
float inArray[5];
int digit;
int LED = 13;

//Display Setup
//UTFT myGLCD(MI0283QT9,11,13,7,8);               // Watterott MI0283QT9
//UTFT myGLCD(DMTFT28105,MOSI,SCK,10,NOTINUSE,9); // DisplayModule DM-TFT28-105 (Works with both Arduino Uno and Arduino Mega)
//UTFT myGLCD(TFT01_22SP,9,8,12,11,10);           // ElecFreaks TFT01-2.2SP
//UTFT myGLCD(TFT01_24SP,9,8,12,11,10);           // ElecFreaks TFT01-2.4SP
UTFT myGLCD(ITDB32WC, 38, 39, 40, 41);          // ElecFreaks TFT2.2SP Shield

UTFT_tinyFAT myFiles(&myGLCD);

URTouch  myTouch( 6, 5, 4, 3, 2);               // Setup touch screen I/O
extern uint8_t ArialNumFontPlus[];
extern uint8_t Arial_round_16x24[];
extern uint8_t SmallFont[];
int x, y;
char stCurrent[8] = "";
int stCurrentLen = 0;

word res;

char* images[] = {"homescrn.RAW", "numpad.RAW"};

// Switch between number pad and home screen
void switchScreen(boolean nPad) {
  myGLCD.clrScr();
  if (nPad){
    myGLCD.setFont(Arial_round_16x24);
    myGLCD.setColor(255, 255, 255);
    myGLCD.setBackColor(0, 0, 0);
    res = myFiles.loadBitmap(0, 80, 240, 240, images[1]);
    //SD file access validation
    //    if (res!=0)
    //      {
    //        if (res==0x10)
    //        {
    //          myGLCD.print("File not found...", 0, 0);
    //          myGLCD.print(images[1], 0, 14);
    //        }
    //        else
    //        {
    //          myGLCD.print("ERROR: ", 0, 0);
    //          myGLCD.printNumI(res, 56, 0);
    //        }
    //      }
  } else {
    myGLCD.setFont(Arial_round_16x24);
    myGLCD.setColor(0, 0, 0);
    myGLCD.setBackColor(255, 255, 255);
    res = myFiles.loadBitmap(0, 0, 240, 320, images[0]);
    //    if (res!=0)
    //      {
    //        if (res==0x10)
    //        {
    //          myGLCD.print("File not found...", 0, 0);
    //          myGLCD.print(images[1], 0, 14);
    //        }
    //        else
    //        {
    //          myGLCD.print("ERROR: ", 0, 0);
    //          myGLCD.printNumI(res, 56, 0);
    //        }
    //      }
  }
}

// Display current parameters on home screen
void setVal(float inArray[5]) {
  myGLCD.printNumI(inArray[0], 15, 99);
  myGLCD.printNumF(inArray[1], 3, 125, 99);
  myGLCD.printNumI(inArray[2], 15, 154);
  myGLCD.printNumF(inArray[3], 3, 125, 154);
  myGLCD.printNumI(inArray[4], 15, 212);
}

// Draw a white frame while a button is touched
void waitForIt(int x1, int y1, int x2, int y2) {
  myGLCD.setColor(0, 0, 0);
  myGLCD.drawRect (x1, y1, x2, y2);
  while (myTouch.dataAvailable())
    myTouch.read();
  myGLCD.setColor(225, 225, 225);
  myGLCD.drawRect(x1, y1, x2, y2);
  delay(100);
}

// Change number in numpad display box
void updateNumStr(char digit) {
  if (stCurrentLen < 8){
    stCurrent[stCurrentLen] = digit;
    stCurrent[stCurrentLen + 1] = '\0';
    stCurrentLen++;
    myGLCD.print(stCurrent, LEFT, 15);
  }else{
    myGLCD.print("BUFFER FULL!\nPRESS CLR", CENTER, 50);
    delay(200);
  }
}

// Clear number in box
void clrNumStr() {
  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(0,0,140,78);
  stCurrent[8] = "";
  stCurrentLen = 0;
  stCurrent[stCurrentLen] = 0;
  myGLCD.setColor(100,100,100);
}

// Send packet for transmission
void sendData(float inArray[5]) {
  myGLCD.clrScr();
  myGLCD.setColor(0, 0, 0);
  inArray[1] = inArray[1]*1000;
  inArray[3] = inArray[3]*1000;

  if((int)inArray[0]==0 && (int)inArray[1]==0 && (int)inArray[2]==0 && (int)inArray[3]==0){
    int powerTime = (int)inArray[4];
  //  Debug Printout  
//    Serial.print(powerTime);
    Serial3.print(powerTime);
  }else{
    for(int i=0; i<5; i++){
      Serial3.print((int)inArray[i]);
  //  Debug Printout  
//      Serial.print((int)inArray[i]);
      digitalWrite(13,HIGH);
      delay(50);
      digitalWrite(13,LOW);
      delay(100);
      Serial3.print(',');
  //  Debug Printout  
//      Serial.print(',');
    }
  }
  Serial3.print('\n');
//Debug Printout  
//  Serial.print('\n');
  myGLCD.print("SENT", CENTER, 50);
  delay(2000);
  
  switchScreen(false);
  inArray[1] = inArray[1]/1000;
  inArray[3] = inArray[3]/1000;
  setVal(inArray);
}

//Input parameter value using numpad
float numPad(int n)
{
  clrNumStr();
  myGLCD.setColor(100, 100, 100);
  myGLCD.setFont(Arial_round_16x24);
  if (n == 0) {
    myGLCD.print("ms 1 ON", RIGHT, 50);
  } else if (n == 1) {
    myGLCD.print("s 1 OFF", RIGHT, 50);
  } else if (n == 2) {
    myGLCD.print("ms 2 ON", RIGHT, 50);
  } else if (n == 3) {
    myGLCD.print("s 2 OFF", RIGHT, 50);
  } else if (n == 4) {
    myGLCD.print("Cycles", RIGHT, 50);
  }
  myGLCD.setFont(ArialNumFontPlus);
  while (true) {
    myTouch.read();
    x = myTouch.getX(); //THIS IS THE SHORT SIDE
    y = myTouch.getY(); //THIS IS THE LONG SIDE

    if ((x >= 185) && (x <= 235)) {          //Column 4
      if ((y >= 205) && (y <= 315)) {        //GO
        waitForIt(0,0,0,0);
        if (n == 4) {
          switchScreen(false);
        }
        return(atol(stCurrent));
      }
      if ((y >= 85) && (y <= 195)) {         //Clear
        waitForIt(0,0,0,0);
        if (stCurrentLen == 0) {
          switchScreen(false);
          return(-1);
        }
        clrNumStr();
      }
    }
    if ((x >= 125) && (x <= 175)) {          //Column 3
      if ((y >= 265) && (y <= 315)) {        //.
        waitForIt(0,0,0,0);
        updateNumStr('.');
      }
      if ((y >= 205) && (y <= 255)) {        //3
        waitForIt(0,0,0,0);
        updateNumStr('3');
      }
      if ((y >= 145) && (y <= 195)) {        //6
        waitForIt(0,0,0,0);
        updateNumStr('6');
      }
      if ((y >= 85) && (y <= 135)) {         //9
        waitForIt(0,0,0,0);
        updateNumStr('9');
      }
    }
    if ((x >= 65) && (x <= 115)) {           //Column 2
      if ((y >= 265) && (y <= 315)) {        //0
        waitForIt(0,0,0,0);
        updateNumStr('0');
      }
      if ((y >= 205) && (y <= 255)) {        //2
        waitForIt(0,0,0,0);
        updateNumStr('2');
      }
      if ((y >= 145) && (y <= 195)) {        //5
        waitForIt(0,0,0,0);
        updateNumStr('5');
      }
      if ((y >= 85) && (y <= 135)) {         //8
        waitForIt(0,0,0,0);
        updateNumStr('8');
      }
    }
    if ((x >= 5) && (x <= 55)) {             //Column 1
      if ((y >= 265) && (y <= 315)) {        //0
        waitForIt(0,0,0,0);
        updateNumStr('0');
      }
      if ((y >= 205) && (y <= 255)) {        //1
        waitForIt(0,0,0,0);
        updateNumStr('1');
      }
      if ((y >= 145) && (y <= 195)) {        //4
        waitForIt(0,0,0,0);
        updateNumStr('4');
      }
      if ((y >= 85) && (y <= 135)) {         //7
        waitForIt(0,0,0,0);
        updateNumStr('7');
      }
    }
    if ((x > 55) && (x < 65)) {              //Handler for larger 0 key
      if ((y >= 265) && (y <= 315)) {        //0
        waitForIt(0,0,0,0);
        updateNumStr('0');
      }
    }
  }
}

void param(int x, int y)
{
  if ((x >= 15) && (x <= 105)) {          //Left Column
    if ((y >= 95) && (y <= 125)) {        //LED1_ON
      waitForIt(10, 95, 110, 125);
      switchScreen(true);
      for(int j=0; j<5; j++){
        inArray[j] = numPad(j);
        if(inArray[j]<0){
          inArray[j]=0;
          break;
        }
      }
      setVal(inArray);
    }
    if ((y >= 148) && (y <= 179)) {        //LED2_ON
      waitForIt(10, 148, 110, 179);
      switchScreen(true);
      for(int j=2; j<5; j++){
        inArray[j] = numPad(j);
        if(inArray[j]<0){
          inArray[j]=0;
          break;
        }
      }
      setVal(inArray);
    }
  }
  if ((x >= 115) && (x <= 205)) {         //Right Column
    if ((y >= 95) && (y <= 125)) {        //LED1_OFF
      waitForIt(110, 95, 210, 125);
      switchScreen(true);
      for(int j=1; j<5; j++){
        inArray[j] = numPad(j);
        if(inArray[j]<0){
          inArray[j]=0;
          break;
        }
      }
      setVal(inArray);
    }
    if ((y >= 148) && (y <= 179)) {        //LED2_OFF
      waitForIt(110, 148, 210, 179);
      switchScreen(true);
      for(int j=3; j<5; j++){
        inArray[j] = numPad(j);
        if(inArray[j]<0){
          inArray[j]=0;
          break;
        }
      }
      setVal(inArray);
    }
  }
  if ((x >= 15) && (x <= 205)) {           //Number of Cycles
    if ((y >= 207) && (y <= 237)) {
      waitForIt(10, 207, 210, 237);
      switchScreen(true);
      for(int j=4; j<5; j++){
        inArray[j] = numPad(j);
        if(inArray[j]<0){
          inArray[j]=0;
          break;
        }
      }
      setVal(inArray);
    }
  }
  if ((x >= 90) && (x <= 150)) {
    if ((y >= 267) && (y <= 298)) {            //Run
      waitForIt(90, 267, 150, 298);
      sendData(inArray);
    }
  }
}

void setup() {

  //9600 baud for the RFduino Transmitter
  Serial.begin(9600);
  Serial3.begin(9600);
  
  //Display initialization
  myGLCD.InitLCD(TOUCH_ORIENTATION);
  myGLCD.clrScr();

  myTouch.InitTouch(TOUCH_ORIENTATION);
  myTouch.setPrecision(PREC_MEDIUM);

  file.initFAT();

  myGLCD.setFont(Arial_round_16x24);
  switchScreen(false);
  for(int i=0;i<5;i++){
    inArray[i] = 0;
  }
  setVal(inArray);
}

void loop() {
  myTouch.read();
  x = myTouch.getX(); //THIS IS THE SHORT SIDE
  y = myTouch.getY(); //THIS IS THE LONG SIDE

  param(x,y);

//  serialEvent3()
//  {
//    char feedback;
//    if (Serial2.available()){
//      feedback = Serial2.read();
//      // get the new bytes:
//  //    for(int i=0; i<2; i++){
//  //      feedback[i] = Serial3.read();
//  //    }
//  //    int temp = feedback[0];
//  //    int state = feedback[1];
//  //    if (state)  myGLCD.print("ERROR", CENTER, 50);
//  //    myGLCD.printNumI(temp, 3, 201, 15);
//      myGLCD.print(feedback, 201, 3);
//    }
//  }

}

//void serialEvent2()
//{
//  char feedback;
//  while (Serial2.available()){
//    feedback = Serial2.read();
//    // get the new bytes:
////    for(int i=0; i<2; i++){
////      feedback[i] = Serial3.read();
////    }
////    int temp = feedback[0];
////    int state = feedback[1];
////    if (state)  myGLCD.print("ERROR", CENTER, 50);
////    myGLCD.printNumI(temp, 3, 201, 15);
//    myGLCD.print(feedback, 201, 3);
//  }
//}
