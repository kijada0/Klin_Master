#include  <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

//------------------------- PIN_DEFINE -------------------------
#define R1_pin 2
#define R2_pin 3
#define L1_pin 8
#define L2_pin 7
#define STBY_pin 4 
#define PWM_R_pin 5
#define PWM_L_pin 6

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
  STBY = true;
  data(); // recive data and convert
  serial(); // serial print data
  control(); // motors control
  time_old = time;
}

if(time - time_old >= 500){
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
  Serial.print("\t\tA_PWM: ");
    Serial.print(A_pwm);
  Serial.print("\tB_PWM: ");
    Serial.print(B_pwm);
  Serial.print("\tA_IO: ");
    Serial.print(A_io);
  Serial.print("\tB_IO: ");
    Serial.print(B_io);
  //PIN  
  Serial.print("\t\tA_PWM: ");
    Serial.print(R_pwm);
  Serial.print("\tB_PWM: ");
    Serial.print(L_pwm);
  Serial.print("\tDir: ");
    Serial.println(Dir);
}

//------------------------- DATA -------------------------
void data(){
radio.read(msg, 16);
//TAB>>>VARIBLE
  A_pwm = msg[0]*100 + msg[1]*10 + msg[2]*1;
  B_pwm = msg[3]*100 + msg[4]*10 + msg[5]*1;
  A_io = msg[6];
  B_io = msg[7];
//RAW>>>USE
  A_pwm = map(A_pwm, 0, 511, -255, 255);
  B_pwm = map(B_pwm, 0, 511, -100, 100);
//PWM CALCULATION 
  if(B_pwm>=0){
    R_pwm = abs(A_pwm)*((100-abs(B_pwm))/100);
    L_pwm = abs(A_pwm);
  }
  else{
    L_pwm = abs(A_pwm)*((100-abs(B_pwm))/100);
    R_pwm = abs(A_pwm);
  }  
//DIR CALCULATION 
  if(A_pwm>=0) R_dir = L_dir = true;
  else R_dir = L_dir = false;
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
if(STBY){
  digitalWrite(STBY_pin, STBY);
//PWM
  analogWrite(PWM_R_pin, R_pwm);
  analogWrite(PWM_L_pin, L_pwm);
//DIR
  digitalWrite(R1_pin, R_dir);
  digitalWrite(R2_pin, !R_dir);
  digitalWrite(L1_pin, L_dir);
  digitalWrite(L2_pin, !L_dir);
}

else{
  digitalWrite(PWM_R_pin, false);
  digitalWrite(PWM_L_pin, false);
  digitalWrite(STBY_pin, false);
}
}
