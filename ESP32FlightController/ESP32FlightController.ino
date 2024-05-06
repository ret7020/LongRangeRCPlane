#include <ESP32Servo.h>
#include "config.h"
#include <WiFi.h>

// Runtime variables
byte loraData[8];
byte prev = 0;
int pos;
unsigned long long last = millis();
uint16_t freqSum = 0;
uint8_t freqCount = 0;
uint8_t lastAvgFreq = 0;

// TODO Array
Servo leftAileron;
Servo rightAileron;
Servo elevator;

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
    printf("LoRa config check at %d baud", LORA_CONFIG_BAUD, Serial);
#endif

    // Radio setup
    // Read LoRa config on 9600 baud

    pinMode(MODE_PIN, OUTPUT);

#ifdef DEBUG
    Serial2.begin(9600, SERIAL_8N1, 16, 17);
    printf("Writing 1 to pin %d", MODE_PIN, Serial);
    digitalWrite(MODE_PIN, 1); // Config mode
    
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
    printf("Switching LoRa to working baud %d", LORA_WORKING_BAUD, Serial);
    Serial2.flush();
    delay(1000);
    Serial2.end();
    delay(1000);
#endif DEBUG
    Serial2.begin(LORA_WORKING_BAUD, SERIAL_8N1, 16, 17);
    Serial2.setTimeout(100);
#ifdef DEBUG
    Serial.println("LoRa Ready to switch into recieve mode");
#endif

#ifdef HTTP_DEBUG
    WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PSK);
    IPAddress IP = WiFi.softAPIP();
    #ifdef DEBUG    
        Serial.print("AP IP address: ");
        Serial.println(IP);
        server.begin();
    #endif
#endif

    digitalWrite(MODE_PIN, 0); // Normal mode
    delay(1000);
    Serial2.flush();
}

void loop()
{
    // LoRa recieve part
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
    #endif
                freqSum = 0;
                freqCount = 0;
            }
            prev = loraData[4];
            last = millis();
        }
        else
        {
            prev = incomingByte;
        }
    }

    // Ground WiFi
    #ifdef HTTP_DEBUG
        WiFiClient client = server.available();

        if (client) {                     
            #if DEBUG        
            Serial.println("New Client.");
            #endif
            String currentLine = "";
            while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                #ifdef DEBUG
                Serial.write(c);
                #endif
                header += c;
                if (c == '\n') {
                    if (currentLine.length() == 0) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println("Connection: close");
                        client.println();
                        client.println();
                        client.println(lastAvgFreq);

                        break;
                    } else { 
                        currentLine = "";
                    }
                } else if (c != '\r') {
                    currentLine += c;
                }
            }
        }
        // Clear the header variable
        header = "";
        // Close the connection
        client.stop();
        Serial.println("Client disconnected.");
        Serial.println("");
    }

    #endif
}