#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <AFMotor.h>
#include <Servo.h>
#include <AccelStepper.h>


// F: Front, B: Back, L: Left, R: Right
AF_DCMotor motor1(1);
AF_DCMotor motor2(2);
AF_DCMotor motor3(3);
AF_DCMotor motor4(4);
Servo ESC;

const int stepPin = 44; 
const int dirPin = 45; 

RF24 radio(48, 49);  // CE, CSN

char transmitData[20]={0};
char transmitDataB[2]={0};
const byte address[6] = "00001";
int xLeftJoy,yLeftJoy,xRightJoy,yRightJoy;
int buttonLeftTop,buttonRightTop, buttonLeftBot, buttonRightBot;
unsigned long time,temporTime;
bool flagStep=false;

AccelStepper stepper(1,stepPin,dirPin);  
void setup(){
  motor1.setSpeed(200);
	motor1.run(RELEASE);
  motor2.setSpeed(200);
	motor2.run(RELEASE);
  motor3.setSpeed(200);
	motor3.run(RELEASE);
  motor4.setSpeed(200);
	motor4.run(RELEASE);

  // pinMode(stepPin,OUTPUT); 
  // pinMode(dirPin,OUTPUT);

  ESC.attach(10,1000,2000); // (pin, min pulse width, max pulse width in microseconds) 
  pinMode(32,OUTPUT);
  Serial.begin(9600);
  time=millis();
  temporTime=millis();
  radio.begin();
  
  //set the address
  radio.openReadingPipe(0, address);
  
  //Set module as receiver
  radio.startListening();

  //stepper
  stepper.setMaxSpeed(100);
  // stepper.setCurrentPosition(0);
}
void receiveData(){
  //Read the data if available in buffer
  Serial.println("________________");
  if (radio.available()){
    radio.read(&transmitData, sizeof(transmitData));
    String temporValue="";
    for (int i =0; i<20;i++){
      if(transmitData[i]!='.'){
        temporValue+= transmitData[i]; 
      }
      // Serial.print(i);
      // Serial.print(" ");
      // Serial.println(temporValue);
      switch (i){
        case 3:
          xLeftJoy= temporValue.toInt();
          temporValue="";
          break;
        case 7:
          yLeftJoy= temporValue.toInt();
          temporValue="";
          break;
        case 11:
          xRightJoy= temporValue.toInt();
          temporValue="";
          break;
        case 15:
          yRightJoy= temporValue.toInt();
          temporValue="";
          break;
        case 16:
          buttonRightTop= temporValue.toInt();
          temporValue="";
          break;
        case 17:
          buttonRightBot= temporValue.toInt();
          temporValue="";
          break;
        case 18:
          buttonLeftBot= temporValue.toInt();
          temporValue="";
          break;
        case 19:
           buttonLeftTop=temporValue.toInt();
          temporValue="";
          break;
        default:
          break;
        }
      
      
      // Serial.print(transmitData[i]);
    }
   
    
  }
}
void showData(){
  Serial.print(xLeftJoy);
  Serial.print(" ");
  Serial.print(yLeftJoy);
  Serial.print(" ");
  Serial.print(xRightJoy);
  Serial.print(" ");
  Serial.print(yRightJoy);
  Serial.print(" ");
  Serial.print(buttonLeftTop);
  Serial.print(" ");
  Serial.print(buttonRightTop);
  Serial.print(" ");
  Serial.print(buttonLeftBot);
  Serial.print(" ");
  Serial.print(buttonRightBot);
  Serial.print(" ");
  Serial.println();
}
void runForward(int velocity=255){
  motor1.setSpeed(velocity);
  motor2.setSpeed(velocity);
  motor3.setSpeed(velocity);
  motor4.setSpeed(velocity);
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}

void runBackward(int velocity=255){
  motor1.setSpeed(velocity);
  motor2.setSpeed(velocity);
  motor3.setSpeed(velocity);
  motor4.setSpeed(velocity);
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
}

void runLeft(int velocity=255){
  motor1.setSpeed(velocity);
  motor2.setSpeed(velocity);
  motor3.setSpeed(velocity);
  motor4.setSpeed(velocity);
  motor1.run(FORWARD);
  motor2.run(BACKWARD);
  motor3.run(FORWARD);
  motor4.run(BACKWARD);
}

void runRight(int velocity=255){
  motor1.setSpeed(velocity);
  motor2.setSpeed(velocity);
  motor3.setSpeed(velocity);
  motor4.setSpeed(velocity);
  motor1.run(BACKWARD);
  motor2.run(FORWARD);
  motor3.run(BACKWARD);
  motor4.run(FORWARD);
}

void runCW(int velocity=255){
  motor1.setSpeed(velocity);
  motor2.setSpeed(velocity);
  motor3.setSpeed(velocity);
  motor4.setSpeed(velocity);
  motor1.run(BACKWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(BACKWARD);
}

void runCCW(int velocity=255){
  motor1.setSpeed(velocity);
  motor2.setSpeed(velocity);
  motor3.setSpeed(velocity);
  motor4.setSpeed(velocity);
  motor1.run(FORWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(FORWARD);
}

void stop(){
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}
void runBLDC(){
  int bldcValue =map(yRightJoy, 530, 0, 0, 180);
  ESC.write(bldcValue);
  // Serial.println(bldcValue);
}
void stepRotateCW(){ 
  while(stepper.currentPosition() != 50)
  {
    stepper.setSpeed(30);
    stepper.runSpeed();
  }
  stepper.setCurrentPosition(0);
}

void stepRotateCCW(){  
  while(stepper.currentPosition() != -50)
  {
    stepper.setSpeed(-30);
    stepper.runSpeed();
  }
  stepper.setCurrentPosition(0);
}

void startProgram(){
  receiveData();
  showData();
  //controll motion
  if (buttonLeftTop==0&& buttonRightTop==0){
    if (yLeftJoy<200){
      runForward();
    }else if(yLeftJoy>900){
      runBackward();
    }else if (xLeftJoy<100){
      runLeft();
    }else if (xLeftJoy>900){
    runRight();
    }else{
      stop();
    }
  }else if (buttonLeftTop==1 && buttonRightTop==0){
    runCCW();
  }else if (buttonLeftTop==0 && buttonRightTop==1){
    runCW();
  }

  if (yRightJoy<530){
    runBLDC();
  }else{
    yRightJoy=530;
    runBLDC();
  }

  if (buttonLeftBot==1&&buttonRightBot==0&&flagStep==true){
    stepRotateCCW(); 
    flagStep=false;
  }else if (buttonLeftBot==0&&buttonRightBot==1&&flagStep==true){
    stepRotateCW();
    flagStep=false;
  }else if (buttonLeftBot==0&&buttonRightBot==0){
    flagStep=true;
  }

}


void loop(){
  //2 1
  //3 4
  startProgram();
  //  stepper.setSpeed(400);
  // Step the motor with a constant speed as set by setSpeed():
  // stepper.runSpeed();




  
  
  


}