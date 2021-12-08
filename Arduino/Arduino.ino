#include <MsTimer2.h>
#include <StopWatch.h>
#include "pitches.h"

int melody[] = {
  NOTE_C5, NOTE_C5, NOTE_D5, NOTE_D5, NOTE_D5, NOTE_A5, NOTE_G5,NOTE_E5, NOTE_F5, NOTE_G5, NOTE_C5, NOTE_C5, NOTE_G5, NOTE_F5, NOTE_F5, NOTE_G5,
  NOTE_A5, NOTE_C6, NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F5, NOTE_F5, NOTE_G5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, 0, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_D5, NOTE_D5, NOTE_A5, NOTE_G5, NOTE_E5, NOTE_F5,
  NOTE_G5, NOTE_C5, NOTE_C5, NOTE_G5, NOTE_F5, NOTE_F5, NOTE_E5, NOTE_D5, NOTE_A5, NOTE_E5, NOTE_C5, NOTE_A5, NOTE_E5, NOTE_G5, 0, 0, 0, NOTE_C6, NOTE_C6, 0, NOTE_C6, NOTE_C6,
  NOTE_C6, NOTE_C6, NOTE_A5, NOTE_A5, NOTE_G5, NOTE_G5, NOTE_C6, NOTE_C6, 0, NOTE_C6, NOTE_C6, 0, NOTE_G5, NOTE_AS5, NOTE_A5,
  
  0, NOTE_C5, NOTE_C5, NOTE_C5, NOTE_C5, NOTE_D5, NOTE_G5, NOTE_F5, NOTE_AS5, NOTE_A5, NOTE_A5, 0, NOTE_F5, NOTE_G5, NOTE_A5, 0, NOTE_F5, NOTE_G5,
  NOTE_A5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_A5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_F5, NOTE_G5, 0, 0, NOTE_F5, NOTE_G5, NOTE_A5, 0, NOTE_F5, NOTE_G5,
  NOTE_A5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_F5, NOTE_G5, 0, NOTE_C5, NOTE_C5, NOTE_A5, NOTE_AS5, NOTE_C5, NOTE_A5, NOTE_G5,
  NOTE_C5, NOTE_C5, NOTE_C5, NOTE_C5, NOTE_A5, NOTE_AS5, NOTE_C5, NOTE_A5, NOTE_A5, NOTE_G5, NOTE_A5,

  NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5, NOTE_G5, NOTE_F5, NOTE_F5, NOTE_G5, NOTE_F5, NOTE_F5, NOTE_G5, NOTE_F5, NOTE_F5, NOTE_G5, NOTE_F5, NOTE_F5, NOTE_G5, NOTE_F5, NOTE_A5, 0,
  0, NOTE_F5, 0, NOTE_F5, NOTE_D5, NOTE_F5, NOTE_C5, NOTE_E5, NOTE_F5, NOTE_F5, NOTE_C5, NOTE_F5, NOTE_F5, NOTE_C5, NOTE_G5, NOTE_F5, NOTE_F5, 0, 0, NOTE_C5, NOTE_C5, NOTE_A5,
  NOTE_C5, NOTE_C5, NOTE_C5, NOTE_A5, NOTE_G5, NOTE_F5, NOTE_G5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_D5, NOTE_D5, NOTE_A5, NOTE_G5, NOTE_E5, NOTE_F5,
  NOTE_G5, NOTE_C5, NOTE_C5, NOTE_G5, NOTE_F5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_C5, NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F5, NOTE_F5, NOTE_G5, 0, 0, 0
};

float noteDurations[] = {
  8,8,8,8,8,8,4,8,8,8,8,8,8,4,8,8,
  8,8,8,8,8,8,8,8,8,8,8,8,4,8,8,8,8,8,8,4,8,8,
  8,8,8,8,4,8,8,8,4,8,8,4,8,4,4,2,8,8,4,8,8,4,
  8,8,16,16,16,16,8,4,8,8,4,16,16,8,4,
  
  8,16,16,16,16,8,8,8,8,8,4,8,16,16,4,8,16,16,
  8,16,16,16,16,16,16,8,4,8,4,8,16,16,4,8,16,16,
  8,16,16,8,16,16,8,4,8,8,8,8,16,16,8,8,4,
  16,16,8,8,16,16,8,16,16,8,8,

  8,16,16,16,8,16,16,8,16,16,8,16,16,8,16,16,8,16,4,4,
  4,4,8,16,16,4,4,16,16,16,16,16,16,16,8,8,4,4,8,16,16,4,
  16,8,16,8,8,8,4,8,8,8,8,8,8,4,8,8,
  8,8,8,8,4,8,8,8,8,8,8,8,8,8,8,4,4,4
};

StopWatch MySW;
int trackingIR = 2;  // IR 트래킹 모듈
int toiletSW = 3;     // 화장실 스위치

int red = 9;
int green = 10;
int blue = 11;

String id = "2017154003";
String seat = "X";    // 착석 
String toilet = "X";  // 화장실
String data = "";
int attend = 0;

void setup() {
  Serial.begin(9600);
  pinMode(toiletSW, INPUT_PULLUP);
  //pinMode(trackingIR, INPUT);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(toiletSW), toiletCnG, HIGH);
} // setUp

void loop() {
  int sensor = digitalRead(trackingIR);

  MySW.start(); // 스탑워치 시작

  // 사람이 없는 경우
  if(MySW.elapsed()>=5000 && sensor==HIGH){ 
      seat = "X";
      data = id + seat + toilet;
      //Serial.println(sensor);
      
      Serial.println(data);
      MySW.reset();

      if(toilet == "O"){ attend += 5; }
      
      colorLED();
      piezoBuzzer();
  } // out
  
  // 사람이 있는 경우
  else if(MySW.elapsed()>=5000 && sensor==LOW){
      seat = "O";
      toilet = "X";
      data = id + seat + toilet;
      //Serial.println(sensor);
      Serial.println(data);
      MySW.reset();

      colorLED();
  } // out
  notAttend();
}

// 화장실 갈 때 누르는 버튼
void toiletCnG() {
  if(toilet == "O") {
    toilet = "X";
  } else{
    toilet = "O";
  }
}

// 자리를 오래 비워 결석 처리
void notAttend() {
  if(attend > 60){
    data = id + String(attend);
    Serial.println(data);
  }
}

// LED 초기화
void initLED() {
  digitalWrite(red, LOW);
  digitalWrite(green, LOW);
  digitalWrite(blue, LOW);
}

// 상황에 따라 LED 색깔 변화
void colorLED() {
  if(seat == "O"){
    initLED();
    digitalWrite(green, HIGH);
  }
  if(toilet == "O"){
    initLED();
    digitalWrite(green, HIGH);
    digitalWrite(red, HIGH);
  } else if(seat == "X" && toilet == "X"){
    initLED();
    digitalWrite(red, HIGH);
  }
}

// 자리에 없고, 화장실 버튼을 안눌렀을 때
// 피에조 부저를 통해 사운드 재생
void piezoBuzzer(){
  int thisNote = 0;

  int size = sizeof(melody) / sizeof(int);
  while(thisNote < size){
    if(toilet == "O"){ noTone(8); break; }
    int noteDuration = 1000 / noteDurations[thisNote]*2;
    tone(8, melody[thisNote], noteDuration);
    delay(noteDuration * 1.30);
    
    if(digitalRead(trackingIR)==HIGH){ thisNote++; } 
    else{ noTone(8); break; }

    if(thisNote==size-1){ thisNote = 0; }
  } // while
}
