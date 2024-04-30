#include <SoftwareSerial.h>


byte loraData[8];
int loraLastPacketReadTime = millis();

SoftwareSerial loraSerial(2, 3);

void setup(){
	Serial.begin(9600);
	loraSerial.begin(9600);

	pinMode(4, OUTPUT);
	digitalWrite(4, 1); // Config mode

	delay(2000);

	// Request to read 6 config regs	
	loraSerial.write(byte(0xC1));
	loraSerial.write(byte(0x00));
	loraSerial.write(byte(0x06));

	delay(2000);

	while (loraSerial.available()){
		Serial.print(loraSerial.read(), HEX);
		Serial.print(" ");
	}
	Serial.println();

	digitalWrite(4, 0); // Normal mode
	
}
void loop(){
	if (loraSerial.available() >= 8){
		loraSerial.readBytes(loraData, 8);
		Serial.println(1 / ((millis() - loraLastPacketReadTime) / 1000));
		loraLastPacketReadTime = millis();
	}
}
