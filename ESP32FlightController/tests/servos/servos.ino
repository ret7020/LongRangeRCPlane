#include <ESP32Servo.h>

Servo servo0;
Servo servo1;
Servo servo2;
Servo servo3;
uint8_t currAngle = 90;
long long lastMove = 0;

void logInfo(String text)
{
    Serial.print("[INFO] ");
    Serial.println(text);
}

void setup()
{
    Serial.begin(115200);

    logInfo("Attaching");
    servo0.attach(12);
    servo1.attach(14);
    servo2.attach(27);
    servo3.attach(26);

    logInfo("Write start poses");
    // Start pose
    servo0.write(currAngle);
    servo1.write(currAngle);
    servo2.write(currAngle);
    servo3.write(currAngle);
    logInfo("Start poses applied");
}

void loop()
{
    for (uint8_t pos = 0; pos <= 180; pos += 1)
    { 
        servo0.write(pos);
        servo1.write(pos);
        servo2.write(pos);
        servo3.write(pos);
        delay(5);
    }
    for (uint8_t pos = 180; pos >= 0; pos -= 1)
    {
        servo0.write(pos);
        servo1.write(pos);
        servo2.write(pos);
        servo3.write(pos);
        delay(10);
    }
}