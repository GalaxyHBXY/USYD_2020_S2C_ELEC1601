#include <Servo.h> 

Servo servoLeft;  
Servo servoRight;  
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
int irDetect(int irLedPin, int irReceiverPin, long frequency) {
  tone(irLedPin, frequency, 8);              // IRLED 38 kHz for at least 1 ms   
  delay(1); // Wait 1 ms   
  int ir = digitalRead(irReceiverPin);       
  // IR receiver -> ir variable   
  delay(1);                                  
  // Down time before recheck   
  return ir;                                 
  // Return 1 no detect, 0 detect 
  }   
void setup() {   
  servoLeft.attach(13);   
  servoRight.attach(12);       
  pinMode(3, INPUT);   
  pinMode(2, OUTPUT);   // Left IR LED & Receiver      
  pinMode(10, INPUT);   
  pinMode(9, OUTPUT);     
  Serial.begin(9600);    // put your setup code here, to run once:  
  }  

void loop() {
  int irLeft = irDetect(9, 10, 38000);   
  int irRight = irDetect(2, 3, 38000); 
    Serial.print("left: ");   
    Serial.println(irLeft);   
    Serial.println(irRight);   
  if(irLeft == 0){
    right();
    
    //    Serial.print("left: ");   
    //    Serial.println(irLeft);   
    //    Serial.println(irRight);   
    }   
  else if(irRight == 0){
    left();
    }   
  else{
    forward();   
    }   
  delay(100);    // put your main code here, to run repeatedly:  
  }
