#include <Arduino.h>


#define RTRCTL GPIO_NUM_12
#define LEDC_CHNL 0

uint16_t throttle = 0;
uint8_t readed = 51;
unsigned long start_ms = 0;

void setup() {
    Serial.begin(9600);
    // Calibrate ESC
    ledcSetup(LEDC_CHNL, 200, 8);
    ledcAttachPin(RTRCTL, LEDC_CHNL);

    throttle = 51;
    ledcWrite(LEDC_CHNL, throttle);
    delay(2000);
}

void loop(){
    if (Serial.available() > 0) {
        readed = Serial.parseInt();
        if (readed != 0) {
            throttle = readed;
            Serial.println(throttle);
            ledcWrite(LEDC_CHNL, throttle);
        }
    }
}   