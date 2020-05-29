#include "SPI.h"
#include "nRF24L01.h"
#include "RF24.h"

#define a_button 0
#define b_button 3
#define x_axis 2
#define y_axis 1

RF24 radio(8,10);
const uint64_t pipe = 0x5880a1035aLL;

void read_joystick();
void send_data();
void test_serial();

int buf, d;
int msg[8];
 
void setup(){
Serial.begin(9600);

radio.begin();
radio.openWritingPipe(pipe);

Serial.println("Configuration done!");
}
 
void loop(){
  read_joystick();    // read joystick posision;
  send_data();        // send data to reciver
  delay(100);
}

//-------------------- READ JOYSTICK --------------------
void read_joystick(){
//A_BUTTON
  if(!analogRead(a_button)) msg[6] = 1;
  else msg[6] = 0;

//B_BUTTON
  if(!analogRead(b_button)) msg[7] = 0;
  else msg[7] = 1;
  
//X_AXIS
  buf = map(analogRead(x_axis), 0, 1023, 0, 255);
    for(int t=0; t<=2; t++){
      d = buf/pow(10,2-t);
      msg[t] = d%10;
    }
  buf = 0;
  
//Y_AXIS
  buf = map(analogRead(y_axis), 0, 1023, 0, 255);
    for(int t=0; t<=2; t++){
      d = buf/pow(10,2-t);
      msg[t+3] = d%10;
    }
  buf = 0;
}

//---------------------- SEND DATA ----------------------
void send_data(){
  //test_serial();
  radio.write(msg, 16);
}

//-------------------- SERIAL WRITE ---------------------
void test_serial(){
  Serial.println("MSG CONTENT");
  Serial.print(msg[0]);
  Serial.print(msg[1]);
  Serial.print(msg[2]);
    Serial.print(" ");
  Serial.print(msg[3]);
  Serial.print(msg[4]);
  Serial.print(msg[5]);
    Serial.print(" ");
  Serial.print(msg[6]);
  Serial.println(msg[7]);
    Serial.println("");
}
