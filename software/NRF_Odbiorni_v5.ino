#include  <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

//------------------------- PIN_DEFINE -------------------------
// RIGHT
#define R1_pin 2
#define R2_pin 3
#define PWM_R_pin 5
//LEFT
#define L1_pin 7
#define L2_pin 8
#define PWM_L_pin 6
//CONTROL
#define STBY_pin 4 

//------------------------- FUNCION_DEFINE -------------------------
void serial();
void data();
void control();

//------------------------- RF SETUP -------------------------
RF24 radio(9,10); // zajete piny: 9, 10, 11, 12, 13
const uint64_t pipe = 0x5880a1035aLL;

//------------------------- VARIBLE -------------------------
// RECIVE DATA
int msg[8];
float A_pwm, B_pwm;
int a_pwm, b_pwm;
bool A_io, B_io;

//MOTOR CONTROL
float R_pwm, L_pwm, pwm_buf;
bool R_dir, L_dir;
bool Dir, STBY;

//TIME
unsigned long time, time_old;

//------------------------- SETUP -------------------------
void setup() {
//PIN BEGIN
pinMode(R1_pin, OUTPUT);
pinMode(R2_pin, OUTPUT);
pinMode(L1_pin, OUTPUT);
pinMode(L2_pin, OUTPUT);
pinMode(STBY_pin, OUTPUT);
pinMode(PWM_R_pin, OUTPUT);
pinMode(PWM_L_pin, OUTPUT);

//RF BEGIN
radio.begin();
radio.openReadingPipe(1,pipe);
radio.startListening();

//TIME
time = millis();

//SERIAL BEGIN
Serial.begin(9600);
Serial.println("Configuration done!");
}

//------------------------- LOOP -------------------------
void loop() {
if ( radio.available() ){
  data(); // recive data and convert
  serial(); // serial print data
  control(); // motors control
  time_old = time;
}

if(time - time_old >= 250){
  time_old = time;
  STBY = false;
    digitalWrite(STBY_pin, STBY);
    digitalWrite(PWM_R_pin, false);
    digitalWrite(PWM_L_pin, false);
  Serial.println("!!! NO SIGNAL !!!");
    Serial.println();
}
time = millis();
}

//------------------------- SERIAL -------------------------
void serial(){
  Serial.print("VALUE: ");
  //RAW
  Serial.print("\tRAW_PWM: ");
    Serial.print(a_pwm);  Serial.print(" ");  Serial.print(b_pwm); Serial.print("   ");
      Serial.print("\tCOV_PWM: ");
    Serial.print(A_pwm);  Serial.print(" ");  Serial.print(B_pwm);
  Serial.print("\tIO: ");
    Serial.print(A_io);   Serial.print(" ");  Serial.print(B_io);
  //PIN  
  Serial.print("\tPWM: ");
    Serial.print(R_pwm);  Serial.print(" ");  Serial.print(L_pwm);
  Serial.print("\tDir: ");
    Serial.print(R_dir);  Serial.print(L_dir);
  Serial.print("\tSTBY: ");
    Serial.println(STBY);
}

//------------------------- DATA -------------------------
void data(){
radio.read(msg, 16);
//TAB>>>VARIBLE
  A_pwm = msg[0]*100 + msg[1]*10 + msg[2]*1;    a_pwm = A_pwm;
  B_pwm = msg[3]*100 + msg[4]*10 + msg[5]*1;    b_pwm = B_pwm;

  A_io = msg[6];
  B_io = msg[7];
  
//RAW>>>USE
  A_pwm = map(A_pwm, 0, 511, -255, 255);
  B_pwm = map(B_pwm, 0, 511, -255, 255);
  
//PWM CALCULATION 
if(abs(A_pwm)>=10 || abs(B_pwm)>=10){
  STBY = true;
//---------- Rotation ----------
  if(abs(A_pwm)<=32 && abs(B_pwm)>=240){
    R_pwm = L_pwm = abs(B_pwm);
    if(B_pwm>=0){
      R_dir = true;
      L_dir = false;
    }
    else{
      R_dir = false;
      L_dir = true;
    }
  }
//---------- Normal ----------
  else{
    B_pwm = map(B_pwm, -255, 255, -100, 100);
//---------- Center ----------
    if(abs(B_pwm)<=5){
    R_pwm = L_pwm = abs(A_pwm);
    }
//---------- Side ----------
    else{
      if(B_pwm>=0){
        R_pwm = abs(A_pwm)*((115-abs(B_pwm))/100);
        L_pwm = abs(A_pwm);
      }
      else{
        L_pwm = abs(A_pwm)*((115-abs(B_pwm))/100);
        R_pwm = abs(A_pwm);
      } 
    }
//---------- Direcion ----------
    if(A_pwm>=0){
      R_dir = true;
      L_dir = true;
    }
    else{
      R_dir = false;
      L_dir = false;
    }
  }
}
else STBY = false; // stop

//SIDE CHANGE
  if(!B_io){
    pwm_buf = R_pwm;
    R_pwm = L_pwm;
    L_pwm = pwm_buf;
    R_dir = !R_dir;
    L_dir = !L_dir;
  }
}

//------------------------- CONTROL -------------------------
void control(){
//MOTOR ENEBLE
  digitalWrite(STBY_pin, STBY);
//PWM
  analogWrite(PWM_R_pin, R_pwm);
  analogWrite(PWM_L_pin, L_pwm);
//DIR
  digitalWrite(R1_pin, R_dir);
  digitalWrite(R2_pin, !R_dir);
  digitalWrite(L1_pin, L_dir);
  digitalWrite(L2_pin, !L_dir);


if(STBY==false){
  digitalWrite(PWM_R_pin, false);
  digitalWrite(PWM_L_pin, false);
  digitalWrite(STBY_pin, false);
}
}
