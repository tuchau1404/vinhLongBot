#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <string.h>
RF24 radio (7,8);
const byte address[6] = "00001";
const int xLeftJoy=A1;
const int yLeftJoy=A0;
const int xRightJoy=A3;
const int yRightJoy=A2;
char transmitData[20]={0};
char transmitDataB[2]={0};
String strButtonValue;

void setup() {
   radio.begin();
  //set the address
  radio.openWritingPipe(address);
  //Set module as transmitter
  radio.stopListening();
  // Serial.begin(9600);
  DDRD &= !B00111100; // set d2->d5 as INPUTs
  DDRC &= !B001111;
}
void processDataJoy(int pinJoy,int start, int end){
  char temporData[4]="";
  String joyValue;
  joyValue =analogRead(pinJoy);
  joyValue.toCharArray(temporData,5);
  int count0 =0;
  for (int j=start;j<end;j++){
    if (temporData [count0]!=NULL){
      transmitData[j]=temporData[count0];
    }else{
      transmitData[j]='.';
    }
    count0++;  
  }
  
}
void processDataButton(){
  byte read =PIND;
  int value2 =read>>2 &B00000100>>2;
  char value2C= value2 + '0';
  int value3 =read>>3 &B00001000>>3;
  char value3C= value3 + '0';
  int value4 =read>>4 &B00010000>>4;
  char value4C= value4 + '0';
  int value5 =read>>5 &B00100000>>5;
  char value5C= value5 + '0';
  transmitData[16]=value2C;
  transmitData[17]=value3C;
  transmitData[18]=value4C;
  transmitData[19]=value5C;  
  

}
void showData(){
  for (int i =0; i<18;i++){
    if (transmitData[i]!=NULL){
      switch (i){ 
      case 0:
        Serial.println();
        Serial.print("xL: ");
        break;
      case 4:
        Serial.println();
        Serial.print("yL: ");
        break;      
      case 8:
        Serial.println();
        Serial.print("xR: ");
        break;
      case 12:
        Serial.println();
        Serial.print("yR: ");
        break;
      case 16:
        Serial.println();
        Serial.print("button status:");
        break;
      default:
        break;
      
      }
      Serial.print(transmitData[i]);
    }
    
  }
  Serial.println();
  
}
void readData(){
 
  processDataJoy(xLeftJoy,0,4); 
  processDataJoy(yLeftJoy,4,8);
  processDataJoy(xRightJoy,8,12);
  processDataJoy(yRightJoy,12,16);  
  processDataButton();
  
}


void startProgram(){
  // const char text[] = "Hello World";
  readData();
  radio.write(&transmitData,sizeof(transmitData));
}
void loop() {
  startProgram();
 
  
}