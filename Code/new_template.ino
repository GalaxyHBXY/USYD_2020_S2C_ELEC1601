#include<StackArray.h>
#include<Servo.h>

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
int currentMode;
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
  // put your setup code here, to run once:
  currentMode = 0;
  servoLeft.attach(13);
  servoRight.attach(12);
  pinMode(3, INPUT);
  pinMode(2, OUTPUT);//Left IR LED & Receiver
  pinMode(10, INPUT);
  pinMode(9, OUTPUT);//Right IR LED & Receiver
  Serial.begin(9600);
  attachInterrupt(2, modeChange, )//button is pressed
}

void controlling(){
  int x_value = //x 0-1023
  int y_value = //y 0-1023
  int z_value = //z
  if(//摇杆在10：30方向到1：30方向时){
    forward();
    instruction.push(1);
  } else if(//摇杆在7：30方向到10：30方向时){
    left();
    instruction.push(2);
  } else if (//摇杆在1：30方向到4：30方向时){
    right();
    instruction.push(3);
  }
  //如果接收左转就存入右转，接收右转就存入左转
  if(//z被按下时的值域){
    modeChange();
}

void tracing(){
  int irLeft = irDetect(9, 10, 38000);
  int irRight = irDetect(2, 3, 38000);
  if(irLeft == 1 && irRight == 1){
    stop();
  } if(irLeft == 1){
    left();
  } else if (irRight == 1){
    right();
  }
}

void autoback(){
  while(!instruction.isEmpty()){
    int inst = instruction.pop();
    switch(inst){
      case 1:{
        forward();
        break;
      }
      case 2:{
        right();
        break;
      }
      case 3:{
        left();
        break;
      }
    }
  }
  delay()//这里秒数和master发送间隔要一致
}

void loop() {
  if(currentMode = 0){
    controlling();
  } else if (currentMode = 1){
    tracing();
  } else {
    autoback();
  }
}
