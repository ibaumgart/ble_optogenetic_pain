#include <RFduinoGZLL.h>
//#include <RFduino

device_t role = DEVICE0;

// LED pins
const int led_1 = 0;
const int led_2 = 1;
bool sleepyBab = false;

// Indicator for complete experiment
bool complete = false;

// Delay time
int powerTime = 20;

void parseStr(String inStr, int len){
//  digitalWrite(led_1,HIGH);
//  delay(50);
//  digitalWrite(led_1,LOW);
  int k=0;
  String tmpStr;
  int inArray[5];
  for(int i=0; i<len; i++){
    if(inStr[i] == ','){
      inArray[k] = tmpStr.toInt();
      tmpStr = "";
      k++;
    }else{
      tmpStr += inStr[i];
    }
  }
  runPulse(inArray);
}

void runPulse(int inArray[5])
{ 
  for(int i=0; i<inArray[4]; i++){
    digitalWrite(led_1, HIGH);
    delay(inArray[0]);
    digitalWrite(led_1, LOW);
    delay(inArray[1]);
    digitalWrite(led_2, HIGH);
    delay(inArray[2]);
    digitalWrite(led_2, LOW);
    delay(inArray[3]);
  }
  complete = true;
}

void powerDown(String inStr, int len){
  String tmpStr;
  for(int i=0; i<len; i++){
    if(inStr[i] == '\n'){
      powerTime = tmpStr.toInt();
    }else{
      tmpStr += inStr[i];
    }
  }
  digitalWrite(led_1,HIGH);
  delay(50);
  digitalWrite(led_1,LOW);
//  RFduinoGZLL.end();
//  RFduino_ULPDelay(SECONDS(powerTime));//powerTime);
  sleepyBab = true;
//  RFduinoGZLL.begin(role);
}

void setup()
{
  // set opto LEDs as output
  pinMode(led_1, OUTPUT);
  pinMode(led_2, OUTPUT);

  RFduinoGZLL.txPowerLevel = +4;  // (-20dbM to +4 dBm) IB: I'm not sure if the arguement takes "+4" or just "4"
  
  // start the GZLL stack
  RFduinoGZLL.begin(role);
}

void loop() 
{
  delay(2000);
//  sleepyBab = true;
  if(sleepyBab){
    RFduino_ULPDelay(MINUTES(powerTime));
    sleepyBab = false;
  }
  int temp = RFduino_temperature(FAHRENHEIT); // returns temperature and stores in float temp 
  char outArray[2];
  outArray[0] = (char)complete;
  outArray[1] = (char)temp;
//  RFduinoGZLL.sendToHost(NULL,1);
  RFduinoGZLL.sendToHost(outArray,2);
  complete = true;
  // ensure LEDs are off
  digitalWrite(led_1,HIGH);
  delay(10);
  digitalWrite(led_1, LOW);
  digitalWrite(led_2, LOW);
//  delay(1000);
//  RFduino_ULPDelay(powerTime);
  powerTime = 5;
}


void RFduinoGZLL_onReceive(device_t device, int rssi, char *data, int len)
{
  // check if data length is correct
  String inStr = "";
  if(data[len-1]=='\n'){//if(inStr[len-1]=='\n'){
//    Debug Flash
//    digitalWrite(led_1,HIGH);
//    delay(1000*len);
//    digitalWrite(led_1, LOW);
//    delay(50);
//    ifinStr[inStr.length()-1]=='\n'){
      inStr += data;
      if(inStr[len-2] == ','){
        parseStr(inStr, len);
      }else{
        powerDown(inStr,len);
      }
  }else{
    complete = false;
  }
}

