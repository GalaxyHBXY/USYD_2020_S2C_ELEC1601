#include<StackArray.h>
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
Servo servoLeft;
Servo servoRight;
int currentMode=0;
StackArray<int> instruction;//保存指令
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

void controlling(){
    char recvChar;
      if(blueToothSerial.available())   // Check if there's any data sent from the remote Bluetooth shield
      {
          recvChar = blueToothSerial.read();
          Serial.print(recvChar);
          if(recvChar == 'w'){
            forward();
            instruction.push(FORWARD);
            
          }else if(recvChar == 'a'){
            left();
            instruction.push(TURNLEFT);
          }
          else if(recvChar == 'd'){
            right();
            instruction.push(TURNRIGHT);
          }
          else if(recvChar == 's'){
//            right();
//instruction.push(BACKWARD);  
          }
          else if(recvChar == 'b'){
            modeChange();
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
  else if(irLeft == 1){
    left(); 
    }   
  else if(irRight == 1){
    right();
    }    
  delay(100);    // put your main code here, to run repeatedly: 
}

void autoback(){
  while(!instruction.isEmpty()){
    int inst = instruction.pop();
    Serial.println(inst);
//    int inst = instruction.pop();
//    switch(inst){
//      case 1:{
//        forward();
//        break;
//      }
//      case 2:{
//        right();
//        break;
//      }
//      case 3:{
//        left();
//        break;
//      }
//    }
  }
  delay(200);//这里秒数和master发送间隔要一致
}

void loop() {
  if(currentMode == 0){
    controlling();
  } else if (currentMode == 1){
    tracing();
  } else {
    autoback();
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
