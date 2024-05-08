#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <ESP8266WebServer.h>
#include "printf.h"
#include "config.h"


extern "C" {
  #include "user_interface.h"
}


#ifdef WEB_DEBUG
  ESP8266WebServer server(WEB_HTTP_PORT);
#endif
    

RF24 radio(NRF_CE, NRF_CSN);

uint8_t data[NRF_PACKET_ITEMS];
long long controllerLastPacket = 0;
bool ledStatus = 1;
//SoftwareSerial loraSerial(LORA_RX, LORA_TX);



void setup(){
    Serial.begin(115200);
    pinMode(D4, OUTPUT);
    
    bool radioBeginStatus = radio.begin(); 
    
   
    if (!radioBeginStatus) {while (1){}} // Halt if not radio
    
    
    // NRF Setup
    radio.setChannel(NRF_CHANNEL);
    radio.setDataRate(NRF_SPEED);
    radio.setPALevel(NRF_POWER);
    radio.openReadingPipe(0, NRF_ADDRESS);
    radio.startListening();
    
    #ifdef UART_DEBUG
      radio.printDetails();
      Serial.println("NRF Ready");
      Serial.println("Lora setup...");
    #endif
//
//    // Lora Setup
      pinMode(LORA_CONTROL, OUTPUT);
//    #ifdef UART_DEBUG
//      digitalWrite(LORA_CONTROL, 1);
//      delay(2000);
//      Serial.write(byte(0xC0));
//      Serial.write(byte(0x02));
//      Serial.write(byte(0x01));
//      Serial.write(byte(0xE5));
//      delay(2000);
//      while (loraSerial.available()){
//        Serial.print(loraSerial.read(), HEX);
//        Serial.print(" ");
//      }
//    #endif
    digitalWrite(LORA_CONTROL, 0);
//    delay(200);
    

    

    
    
    #ifdef WEB_DEBUG
      Serial.print("WiFi init...");
      String routername = HOSTNAME;
      char* ourname = &routername[0];
      wifi_station_set_hostname(ourname);
      WiFi.begin(WIFI_SSID, WIFI_PASS);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println(WiFi.localIP());
      ESP8266WebServer server(WEB_HTTP_PORT);
      
    #endif
    
    #ifdef WEB_DEBUG
      server.begin();
    #endif
    
    
    
    
}
void loop(){
  if(radio.available()){
      radio.read(&data, sizeof(data));
      if (millis() - controllerLastPacket >= 30){
        Serial.write(byte (0xFF));
        Serial.write(byte (0xFF));
        Serial.write(byte (0x1E));
        Serial.write((byte)strtol(String(data[0], HEX).c_str(), NULL, 16));
        Serial.write((byte)strtol(String(data[1], HEX).c_str(), NULL, 16));
        Serial.write((byte)strtol(String(data[2], HEX).c_str(), NULL, 16));
        Serial.write((byte)strtol(String(data[3], HEX).c_str(), NULL, 16));
        controllerLastPacket = millis();
      }
  }

//  delay(35);

  
  
//      Serial.print(data[0]);
//      Serial.print(" ");
//      Serial.print(data[1]);
//      Serial.print(" ");
//      Serial.print(data[2]);
//      Serial.print(" ");
//      Serial.print(data[3]);
//      Serial.print(" ");
//      Serial.println();
//      dispLED.print(data[0]);
//      myservo.write(map(data[1],0,1023,0,180));
//  }

}
