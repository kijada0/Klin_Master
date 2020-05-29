#include  <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

#define a_pin 8
#define b_pin 6
#define c_pin 5

RF24 radio(9,10);
const uint64_t pipe = 0x5880a1035aLL;

unsigned long time, time_old;

int a_pwm, b_pwm;
bool c_io, d_io;
int msg[8];

void setup() {
Serial.begin(9600);

pinMode(a_pin, OUTPUT);
pinMode(b_pin, OUTPUT);
pinMode(c_pin, OUTPUT);
    digitalWrite(a_pin, true);
    digitalWrite(b_pin, true);
    digitalWrite(c_pin, true);
a_pwm = b_pwm = c_io = d_io = 0;

radio.begin();
radio.openReadingPipe(1,pipe);
radio.startListening();

time = millis();

Serial.println("Configuration done!");
}

void loop() {
if ( radio.available() ){
  radio.read(msg, 16);
  time_old = time;

  a_pwm = msg[0]*100 + msg[1]*10 + msg[2]*1;
  b_pwm = msg[3]*100 + msg[4]*10 + msg[5]*1;
  c_io = msg[6];
  d_io = msg[7];

  analogWrite(a_pin, a_pwm);
  analogWrite(b_pin, b_pwm);
  digitalWrite(c_pin, c_io );
  digitalWrite(c_pin, d_io);

  Serial.println("PIN VALUE");
  Serial.print("A_PWM: ");
    Serial.print(a_pwm);
  Serial.print("\tB_PWM: ");
    Serial.print(b_pwm);
  Serial.print("\tC_IO: ");
    Serial.print(c_io);
  Serial.print("\tD_IO: ");
    Serial.println(d_io);
  }

  if(time - time_old >= 500){
    time_old = time;
    
    digitalWrite(a_pin, true);
    digitalWrite(b_pin, true);
    digitalWrite(c_pin, true);
    
    Serial.println("!!! NO SIGNAL !!!");
    Serial.println();
  }
  time = millis();
}
