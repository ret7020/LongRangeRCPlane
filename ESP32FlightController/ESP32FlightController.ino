#include <ESP32Servo.h>
#include "config.h"
#include <SoftwareSerial.h>
#include <Wire.h>
#include <ESP32Servo.h>
// #include "FastIMU.h"
// #include "filters/Madgwick.h"

#ifdef HTTP_DEBUG
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#endif

EspSoftwareSerial::UART gpsUart;

// Runtime variables
byte loraData[8];
byte prev = 0;
int pos;
unsigned long long last = millis();
uint16_t freqSum = 0;
uint8_t freqCount = 0;
uint8_t lastAvgFreq = 0;

#ifdef IMU_ENABLE
double imuRoll, imuPitch, imuYaw;
double imuQW, imuQX, imuQY, imuQZ;
#endif

// Servos control
// Adjustable for different airplanes configurations
// Indexes are predefined in such order
#define LA_INDEX 0   // Left aileron
#define RA_INDEX 1   // Right aileron
#define ELEV_INDEX 2 // Elevator
// Not used in current plane
#define RUDDER_INDEX 3

Servo servos[4];

#ifdef HTTP_DEBUG
WiFiServer server(HTTP_PORT);
String header;
#endif

// IMU Setup
#ifdef IMU_ENABLE
MPU6500 IMU;
calData calib = {0};
AccelData IMUAccel;
GyroData IMUGyro;
MagData IMUMag;
Madgwick filter;

#endif

// Updates store
uint64_t imuLastUpdate = 0;

#ifdef HTTP_DEBUG
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

int8_t initHTTPStack()
{
    // Return codes:
    // 0  - OK
    // -1 - SPIFS problems
    // -2 - WiFi problems
    if (!SPIFFS.begin(true))
    {
        return -1;
    }

#ifdef WIFI_AP_MODE
    WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PSK);
    IPAddress IP = WiFi.softAPIP();
#ifdef DEBUG
    Serial.print("AP IP address: ");
    Serial.println(IP);
    server.begin();
#endif

#endif
}

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
// Init Servos
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
// Debug UART
#ifdef DEBUG
    Serial.begin(115200);
    delay(1000);
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
#endif
    // If no debug, switch LoRa to working
    Serial2.begin(LORA_WORKING_BAUD, SERIAL_8N1, 16, 17);
    Serial2.setTimeout(100);
#ifdef DEBUG
    Serial.println("LoRa Ready to switch into recieve mode");
#endif

    digitalWrite(MODE_PIN, 0); // Normal mode
    delay(1000);
    Serial2.flush();
    gpsUart.begin(9600, SWSERIAL_8N1, GPS_RX, GPS_TX, false);
}

void loop()
{
    // while (gpsUart.available()){
    //     Serial.write(gpsUart.read());
    // }

    // LoRa recieve part (max freq)
    if (Serial2.available())
    {
        byte incomingByte = Serial2.read();
        if (prev == 255 && incomingByte == 30)
        {
            for (int i = 0; i <= 4; i++)
            {
                loraData[i] = Serial2.read();
            }
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
// We get fresh data
// TODO other servos
#if defined(LA_PIN) && defined(RA_PIN)
            servos[LA_INDEX].write(loraData[3]);
            servos[RA_INDEX].write(loraData[3]); // ROLL_VAL
            
#endif
        }
        else
        {
            prev = incomingByte;
        }
    }

    if (millis() - imuLastUpdate >= IMU_REFRESH_INTERVAL)
    {
    }

// Ground WiFi debug
#ifdef HTTP_DEBUG
    WiFiClient client = server.available();

    if (client)
    {
#if DEBUG
        Serial.println("New Client.");
#endif
        String currentLine = "";
        while (client.connected())
        {
            if (client.available())
            {
                char c = client.read();
#ifdef DEBUG
                Serial.write(c);
#endif
                header += c;
                if (c == '\n')
                {
                    if (currentLine.length() == 0)
                    {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println("Connection: close");
                        client.println();

                        if (header.indexOf("GET /debug/metrics"))
                        {
                        }
                        client.println();
                        client.println(lastAvgFreq);

                        break;
                    }
                    else
                    {
                        currentLine = "";
                    }
                }
                else if (c != '\r')
                {
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