#include <SoftwareSerial.h>

SoftwareSerial ss(D3, D4);

void setup() {
  Serial.begin(9600);
  ss.begin(9600);
  Serial.println("READ");
  pinMode(D2, OUTPUT);
  digitalWrite(D2, 1);
  delay(2000);  
  ss.write(byte(0xC1));
  ss.write(byte(0x00));
  ss.write(byte(0x06));
  delay(2000);
  while (ss.available()){
    Serial.print(ss.read(), HEX);
    Serial.print(" ");
  }
  Serial.println();
 
}

void loop() {
  // put your main code here, to run repeatedly:

}
