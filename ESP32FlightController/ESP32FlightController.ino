#include <ESP32Servo.h>
#include "config.h"
#include <WiFi.h>
#include <SoftwareSerial.h>


EspSoftwareSerial::UART gpsUart;

// Runtime variables
byte loraData[8];
byte prev = 0;
int pos;
unsigned long long last = millis();
unsigned long long lastTelemetrySendTime = 0;
uint16_t freqSum = 0;
uint8_t freqCount = 0;
uint8_t lastAvgFreq = 0;

// Servos control
// Adjustable for different airplanes configurations
// Indexes are predefined in such order
#define LA_INDEX 0 // Left aileron
#define RA_INDEX 1 // Right aileron
#define ELEV_INDEX 2 // Elevator

// Not used in current plane

Servo servos[4] = {};

#ifdef HTTP_DEBUG
WiFiServer server(HTTP_PORT);
String header;
#endif

void printf(char *format, int number, HardwareSerial &refSer)
{
    char buffer[120];
    sprintf(buffer, format, number);
    for (int i = 0; i < strlen(buffer); i++)
    {
        refSer.write(buffer[i]); 
    }
    refSer.write('\n'); 
}

void setup()
{
    
// Debug UART
#ifdef DEBUG
    Serial.begin(115200);
    delay(2000);
    Serial.println("Welcome to ESP32 FC debug");
    Serial.println("[INFO] Servos configuring started");
#endif

// Servos setup
#ifdef LA_PIN
    Servo leftAileron;
    servos[LA_INDEX] = leftAileron;
    servos[LA_INDEX].attach(LA_PIN);
    servos[LA_INDEX].write(SERVOS_INIT_ANGLE);
    
#endif
#ifdef RA_PIN
    Servo rightAileron;
    servos[RA_INDEX] = rightAileron;
    servos[RA_INDEX].attach(RA_PIN);
    servos[RA_INDEX].write(SERVOS_INIT_ANGLE);
#endif
#ifdef ELEV_PIN
    Servo elevator;
    servos[ELEV_INDEX] = elevator;
    servos[ELEV_INDEX].attach(ELEV_PIN);
    servos[ELEV_INDEX].write(SERVOS_INIT_ANGLE);
#endif
#ifdef RUDDER_PIN
    Servo rudder;
    servos[RUDDER_INDEX] = rudder;
    servos[RUDDER_INDEX].attach(RUDDER_PIN);
    servos[RUDDER_INDEX].write(SERVOS_INIT_ANGLE);
#endif

// Motor arming
#ifdef ESC_PIN
    ledcSetup(0, 200, 8);
    ledcAttachPin(ESC_PIN, 0);

    ledcWrite(0, 51);
    delay(2000);
#endif

// Debug UART
#ifdef DEBUG
    Serial.begin(115200);
    delay(1000);
    Serial.println("Welcome to ESP32 FC debug");
#endif

// Radio setup
// Read LoRa config on 9600 baud

#ifdef LORA_900_MODE_PIN
pinMode(LORA_900_MODE_PIN, OUTPUT);
#endif

#ifdef LORA_433_MODE_PIN
pinMode(LORA_433_MODE_PIN, OUTPUT);
#endif


#ifdef DEBUG
    Serial.println("[INFO] RX link configuring started");
    #ifdef LORA_900_RX_LINK
        printf("LoRa config check at %d baud", LORA_900_CONFIG_BAUD, Serial);
    #else
        Serial.println("[FATAL] Can't use other links as RX");
    #endif

    // This state not implemented yet
    #ifdef LORA_433_RX_LINK
        printf("LoRa config check at %d baud", LORA_433_CONFIG_BAUD, Serial);
    #endif
#endif



#ifdef DEBUG

    Serial2.begin(9600, SERIAL_8N1, 16, 17);
    printf("Writing 1 to pin %d", LORA_900_MODE_PIN, Serial);
    digitalWrite(LORA_900_MODE_PIN, 1); // Config mode
    
    delay(2000);

    // Request to read 6 config regs

    Serial2.write(byte(0xC1));
    Serial2.write(byte(0x00));
    Serial2.write(byte(0x06));

    delay(2000);
    Serial.println("Config regs: ");
    while (Serial2.available())
    {
        Serial.print(Serial2.read(), HEX);
        Serial.print(" ");
    }
    Serial.println();
    printf("Switching RX Lora to working baud %d", LORA_900_WORKING_BAUD, Serial);
    Serial2.flush();
    delay(1000);
    Serial2.end();
    delay(1000);
#endif DEBUG
    Serial2.begin(LORA_900_WORKING_BAUD, SERIAL_8N1, 16, 17);
    Serial2.setTimeout(100);
#ifdef DEBUG
    Serial.println("LoRa Ready to switch into recieve mode");    
    delay(2000);
#endif
    digitalWrite(LORA_900_MODE_PIN, 0); // Normal mode
    delay(1000);
    Serial2.flush();
#ifdef DEBUG
    Serial.println("[INFO] RX link ready");
    delay(2000);
#endif




// TX Link

#ifdef DEBUG
    Serial.println("[INFO] TX link configuring started");
    // This state not implemented yet
    #ifdef LORA_433_TX_LINK
        printf("LoRa config check at %d baud", LORA_433_CONFIG_BAUD, Serial);
    #else
        Serial.println("[FATAL] Can't use other links as TX");
    #endif
#endif



#ifdef DEBUG

    Serial1.begin(9600, SERIAL_8N1, LORA_433_USART1_TX, LORA_433_USART1_RX);
    printf("Writing 1 to pin %d", LORA_433_MODE_PIN, Serial);
    digitalWrite(LORA_433_MODE_PIN, 1); // Config mode
    
    delay(2000);

    // Request to read 6 config regs

    // Serial1.write(byte(0xC1));
    // Serial1.write(byte(0x00));
    // Serial1.write(byte(0x06));


    // Setting fixed TX mode
    Serial1.write(byte(0xC0));
    Serial1.write(byte(0x03));
    Serial1.write(byte(0x01));
    Serial1.write(byte(0x3));

    delay(2000);
    Serial.println("Config regs: ");
    while (Serial1.available())
    {
        Serial.print(Serial1.read(), HEX);
        Serial.print(" ");
    }
    Serial.println();
    printf("Switching TX Lora to working baud %d", LORA_433_WORKING_BAUD, Serial);

    


    Serial1.flush();
    delay(1000);
    Serial1.end();
    delay(1000);
#endif DEBUG
    Serial1.begin(9600, SERIAL_8N1, LORA_433_USART1_TX, LORA_433_USART1_RX);
    // Serial1.setTimeout(100);
#ifdef DEBUG
    Serial.println("LoRa Ready to switch into transmit mode");    
    delay(2000);
#endif
    digitalWrite(LORA_433_MODE_PIN, 0); // Normal mode
    delay(1000);
    Serial1.flush();
#ifdef DEBUG
    Serial.println("[INFO] TX link ready");
    delay(2000);
#endif


#ifdef HTTP_DEBUG
    #ifdef DEBUG 
        Serial.println("[INFO] Starting WiFi");
    #endif
    WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PSK);
    IPAddress IP = WiFi.softAPIP();
    #ifdef DEBUG 
        Serial.print("AP IP address: ");
        Serial.println(IP);
        server.begin();
        Serial.println("[INFO] WiFi Ready");
    #endif
#endif
    

// gpsUart.begin(9600, SWSERIAL_8N1, GPS_RX, GPS_TX, false);
}

void loop()
{
    // while (gpsUart.available()){
    //     Serial.write(gpsUart.read());
    // }
    
    // Commands RX
    if (Serial2.available()){
        byte incomingByte = Serial2.read();
        if (prev == 255 && incomingByte == 30)
        {
            loraData[0] = Serial2.read();
            loraData[1] = Serial2.read();
            loraData[2] = Serial2.read();
            loraData[3] = Serial2.read();
            loraData[4] = Serial2.read();
            //        Serial.println(loraData[4]);
            if (freqCount < 20)
            {
                freqSum += millis() - last;
                freqCount += 1;
            }
            else
            {
                lastAvgFreq = freqSum / freqCount;
    #ifdef DEBUG
                Serial.print(lastAvgFreq);
                Serial.print(" ");
                Serial.println("Hz");
                // Serial.println(loraData[4]);
                
    #endif
                freqSum = 0;
                freqCount = 0;
            }
            prev = loraData[4];
            last = millis();
            #if defined(LA_PIN) && defined(RA_PIN)
                servos[LA_INDEX].write(loraData[3]);
                servos[RA_INDEX].write(loraData[3]); // ROLL_VAL
            #endif
            #ifdef ELEV_PIN
                servos[ELEV_INDEX].write(loraData[2]);
            #endif
        }
        else
        {
            prev = incomingByte;
        }
    }

    // Telemetry TX
    // if (millis() - lastTelemetrySendTime >= TELEMETRY_TX_INTERVAL){
    //     Serial1.write(byte (0xFF));
    //     Serial1.write(byte (0xFF));
    //     Serial1.write(byte (0x17));
    //     Serial1.write(byte (0xA));
    //     Serial1.write(byte (0xB));
    //     Serial1.write(byte (0xC));
    //     Serial1.write(byte (0xD));
    //     lastTelemetrySendTime = millis();
        
    // }


    // // Ground WiFi
    // #ifdef HTTP_DEBUG
    //     WiFiClient client = server.available();

    //     if (client) {                     
    //         #if DEBUG        
    //         Serial.println("New Client.");
    //         #endif
    //         String currentLine = "";
    //         while (client.connected()) {
    //         if (client.available()) {
    //             char c = client.read();
    //             #ifdef DEBUG
    //             Serial.write(c);
    //             #endif
    //             header += c;
    //             if (c == '\n') {
    //                 if (currentLine.length() == 0) {
    //                     client.println("HTTP/1.1 200 OK");
    //                     client.println("Content-type:text/html");
    //                     client.println("Connection: close");
    //                     client.println();
    //                     client.println();
    //                     client.println(lastAvgFreq);

    //                     break;
    //                 } else { 
    //                     currentLine = "";
    //                 }
    //             } else if (c != '\r') {
    //                 currentLine += c;
    //             }
    //         }
    //     }
    //     // Clear the header variable
    //     header = "";
    //     // Close the connection
    //     client.stop();
    //     Serial.println("Client disconnected.");
    //     Serial.println("");
    // }

    // #endif
}