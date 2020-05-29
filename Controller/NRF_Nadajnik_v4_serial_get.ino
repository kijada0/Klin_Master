#include "SPI.h"
#include "nRF24L01.h"
#include "RF24.h"


//------------------------- RF SETUP -------------------------
RF24 radio(9,10);
const uint64_t pipe = 0x5880a1035aLL;

//------------------------- FUNCION -------------------------
void send_data();
void test_serial();

//------------------------- VARIBLE -------------------------
int msg[8];
int buff, i;
bool sent;

//TIME
unsigned long time, time_old;

//------------------------- SETUP -------------------------
void setup(){
//SERIAL_BEGIN
Serial.begin(115200);
//RF_BEGIN
radio.begin();
radio.openWritingPipe(pipe);

Serial.println("Configuration done!");
}

//------------------------- LOOP -------------------------
void loop(){
  if (Serial.available()) {
    buff = Serial.read();
    if(i) msg[i-1] = buff - '0';
    i++;
    if(i>=9) i=0;
    time_old = time;
    sent = true;
  }
  
  if(sent && i==0) send_data();        
  
  if(time - time_old >= 500){
    time_old = time;
    for(int j=0; j<=8; j++) msg[j] = 0;
    Serial.println("!!! NO SIGNAL !!!");
      Serial.println();
    sent = false;
  }
  time = millis();
}

//------------------------- SEND DATA -------------------------
void send_data(){
  test_serial();
  radio.write(msg, 16); 
}

//------------------------- SERIAL WRITE -------------------------
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
