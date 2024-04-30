#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "printf.h"

// NRF Settings
#define NRF_CE D1
#define NRF_CSN D8
#define NRF_PACKET_ITEMS 4
#define NRF_CHANNEL 5
#define NRF_SPEED RF24_1MBPS
#define NRF_POWER RF24_PA_LOW
#define NRF_ADDRESS 0x1234567890LL

// WiFi Settings
#define WIFI_SSID MTS_GPON_E80C

// LoRA Settings


#define UART_DEBUG


RF24 radio(NRF_CE, NRF_CSN);

uint8_t data[NRF_PACKET_ITEMS];
long long controllerLastPacket = 0;

void setup(){
    bool radioBeginStatus = radio.begin(); 
    #ifdef UART_DEBUG
      Serial.begin(9600);
      printf_begin();
      Serial.print("Radio begin status: ");
      Serial.println(radioBeginStatus);
      
    #endif
    if (!radioBeginStatus) {while (1){}} // Halt if not radio
    
    
    // NRF Setup
    radio.setChannel(NRF_CHANNEL);
    radio.setDataRate(NRF_SPEED);
    radio.setPALevel(NRF_POWER);
    radio.openReadingPipe(0, NRF_ADDRESS);
    radio.startListening();
    
    #ifdef UART_DEBUG
      radio.printDetails();
    #endif
    
}
void loop(){
  if(radio.available()){
      radio.read(&data, sizeof(data));
        controllerLastPacket = millis();
//      Serial.print(data[0]);
//      Serial.print(" ");
//      Serial.print(data[1]);
//      Serial.print(" ");
//      Serial.print(data[2]);
//      Serial.print(" ");
//      Serial.print(data[3]);
//      Serial.print(" ");
//      Serial.println();
//      myservo.write(map(data[1],0,1023,0,180));
  }

}
