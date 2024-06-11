#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

#define LEFT_VERTICAL A2
#define LEFT_HORIZONTAL A0
#define RIGHT_VERTICAL A1
#define RIGHT_HORIZONTAL A3

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
RF24 radio(7, 8);
int16_t rfData[4] = {90, 90, 90, 90};

void setup()
{
    Serial.begin(9600);
    printf_begin();
    pinMode(LEFT_VERTICAL, INPUT);
    pinMode(LEFT_HORIZONTAL, INPUT);
    pinMode(RIGHT_VERTICAL, INPUT);
    pinMode(RIGHT_HORIZONTAL, INPUT);

    // Display init
    //  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    //  display.display();
    //  display.setTextSize(1);
    //  display.setTextColor(SSD1306_WHITE);
    //  display.setCursor(0,0);
    //  display.println(F("Potentiometers:"));
    //  display.display();

    // NRF24 init
    if (!radio.begin())
    {
        Serial.println(F("radio hardware is not responding!!"));
        while (1)
        {
        } // hold in infinite loop
    }
    else
    {
        Serial.println("Radio OK;");
    }

    delay(500);
    radio.printDetails();
    radio.setChannel(5);
    radio.setDataRate(RF24_2MBPS);
    radio.setPALevel(RF24_PA_LOW);
    radio.openWritingPipe(0x1234567890LL);
    radio.stopListening();
    
}

void loop()
{
    // Read joysticks
    rfData[0] = constrain(map(analogRead(LEFT_VERTICAL), 115, 882, 180, 0), 0, 180);    // LEFT_VERTICAL
    rfData[1] = constrain(map(analogRead(LEFT_HORIZONTAL), 90, 786, 0, 180), 0, 180);  // LEFT_HORIZONTAL
    rfData[2] = constrain(map(analogRead(RIGHT_VERTICAL), 270, 960, 0, 180), 0, 180);   // RIGHT_VERTICAL
    rfData[3] = constrain(map(analogRead(RIGHT_HORIZONTAL), 187, 828, 0, 180), 0, 180); // RIGHT_HORIZONTAL
    
    radio.write(&rfData, sizeof(rfData));



    // Visualize
    //  display.setCursor(0,0);
    //  display.println(F("Potentiometers:"));
    //  display.setCursor(0,10);
    //  display.println(rfData[0]);
    //  display.setCursor(25,10);
    //  display.println(rfData[1]);
    //  display.setCursor(0,25);
    //  display.println(rfData[2]);
    //  display.setCursor(25,25);
    //  display.println(rfData[3]);
    //  display.display();

// Debug to builtin UART

    // Transmit to repeaterNode

}