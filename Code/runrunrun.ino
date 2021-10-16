#include<Servo.h>

#include <SoftwareSerial.h>   //Software Serial Port

#define RxD 7
#define TxD 6
#define ConnStatus A1

#define DEBUG_ENABLED  1


int shieldPairNumber = 8;

boolean ConnStatusSupported = true;   // Set to "true" when digital connection status is available on Arduino pin

String slaveNameCmd = "\r\n+STNA=Slave";   // This is concatenated with shieldPairNumber later

SoftwareSerial blueToothSerial(RxD,TxD);



#define STOP         0
#define FORWARD      1
#define BACKWARD     2
#define TURNLEFT     3
#define TURNRIGHT    4

#define CONTROLMODE  0
#define TRACINGMODE  1
#define AUTOBACKMODE 2
#define AUTOMOVEMODE 3

int stack[500];
int currentIns = 0;
int stackTop = 0;

Servo servoLeft;
Servo servoRight;
int currentMode=0;
void forward(){
  servoLeft.writeMicroseconds(1550);
  servoRight.writeMicroseconds(1450);
}
void left(){
  servoLeft.writeMicroseconds(1300);
  servoRight.writeMicroseconds(1300);
}
void right(){
  servoLeft.writeMicroseconds(1700);
  servoRight.writeMicroseconds(1700);
}
void back(){

}
void stop(){
  
}
int irDetect(int irLedPin, int irReceiverPin, long frequency){
  tone(irLedPin, frequency, 8);
  delay(1);
  int ir = digitalRead(irReceiverPin);
  delay(1);
  return ir;
}

void modeChange(){
  currentMode++;
}

void setup() {
    Serial.begin(9600);
    blueToothSerial.begin(38400);                    // Set Bluetooth module to default baud rate 38400
    
    pinMode(RxD, INPUT);
    pinMode(TxD, OUTPUT);
    pinMode(ConnStatus, INPUT);

    if(ConnStatusSupported) Serial.println("Checking Slave-Master connection status.");

    if(ConnStatusSupported && digitalRead(ConnStatus)==1)
    {
        Serial.println("Already connected to Master - remove USB cable if reboot of Master Bluetooth required.");
    }
    else
    {
        Serial.println("Not connected to Master.");
        
        setupBlueToothConnection();   // Set up the local (slave) Bluetooth module

        delay(1000);                  // Wait one second and flush the serial buffers
        Serial.flush();
        blueToothSerial.flush();
    }
  // put your setup code here, to run once:
  currentMode = 0;
  servoLeft.attach(13);
  servoRight.attach(12);
  pinMode(3, INPUT);
  pinMode(2, OUTPUT);//Left IR LED & Receiver
  pinMode(10, INPUT);
  pinMode(9, OUTPUT);//Right IR LED & Receiver
}

void push(int instruction){
  if(stackTop < 500){
    stack[currentIns] = instruction;
    currentIns++;
    StackTop++;
  }
}

void controlling(){
    char recvChar;
      if(blueToothSerial.available())   // Check if there's any data sent from the remote Bluetooth shield
      {
          recvChar = blueToothSerial.read();
          Serial.print(recvChar);
          if(recvChar == 'w'){
            forward();
            push(FORWARD);
          }else if(recvChar == 'a'){
            left();
            push(TURNLEFT);
          }
          else if(recvChar == 'd'){
            right();
            push(TURNRIGHT);
          }
          else if(recvChar == 's'){
            back();
            push(BACKWARD);
          }
          else if(recvChar == 'b'){
            modeChange();
          } else {
            stop();
            push(STOP);
          }
      }
}

void tracing(){
  char recvChar;
      if(blueToothSerial.available())   // Check if there's any data sent from the remote Bluetooth shield
      {
          recvChar = blueToothSerial.read();
          if(recvChar == 'b'){
            modeChange();
      }
   }
  int irLeft = irDetect(9, 10, 38000);   
  int irRight = irDetect(2, 3, 38000); 
  Serial.print("left: ");   
  Serial.println(irLeft);   
  Serial.println(irRight);
  if(irRight == 0 && irLeft == 0){
    forward();   
  }
  else if(irLeft == 1 && irRight == 1){
    right();
    dealy();//turn back
  }       
  else if(irLeft == 1){
    left(); 
  }   
  else if(irRight == 1){
    right();
  }    
  delay(100);
}

int pop(){
  if(currentIns > 0){
    return stack[currentIns--];
  } else {
    return -1;
  }
}

void autoback(){
  while(1){
    int inst = pop();
    Serial.println(inst);
    if(inst == -1)
      break;
    switch(inst){
      case 0:{
        stop();
        break;
      }
      case 1:{
        forward();
        break;
      }
      case 2:{
        backward();
        break;
      }
      case 3:{
        right();
        break;
      }
      case 4:{
        left();
        break;
      }
    }
    delay(200);
  }
}

void autoMove(){
  while(currentIns < stackTop){
    inst=stack[currentIns];
    switch(inst){
      case 0:{
        stop();
        break;
      }
      case 1:{
        forward();
        break;
      }
      case 2:{
        backward();
        break;
      }
      case 3:{
        left();
        break;
      }
      case 4:{
        right();
        break;
      }
    }
    delay(200);
    currentIns++;
  }
}

void loop() {
  if(currentMode == 0){
    controlling();
  } else if (currentMode == 1){
    tracing();
  } else if (currentMode == 2){
    autoback();
    modeChange();
  } else if (currentMode == 3){
    automove();
    currentMode == 1;
  }
}

void setupBlueToothConnection()
{
    Serial.println("Setting up the local (slave) Bluetooth module.");

    slaveNameCmd += shieldPairNumber;
    slaveNameCmd += "\r\n";

    blueToothSerial.print("\r\n+STWMOD=0\r\n");      // Set the Bluetooth to work in slave mode
    blueToothSerial.print(slaveNameCmd);             // Set the Bluetooth name using slaveNameCmd
    blueToothSerial.print("\r\n+STAUTO=0\r\n");      // Auto-connection should be forbidden here
    blueToothSerial.print("\r\n+STOAUT=1\r\n");      // Permit paired device to connect me
    

    blueToothSerial.flush();
    delay(2000);                                     // This delay is required

    blueToothSerial.print("\r\n+INQ=1\r\n");         // Make the slave Bluetooth inquirable
    
    blueToothSerial.flush();
    delay(2000);                                     // This delay is required
    
    Serial.println("The slave bluetooth is inquirable!");
}
