loraRecv:
	arduino-cli compile ./ForTests/LoraRecieveTest/LoraRecieveTest.ino --fqbn arduino:avr:nano:cpu=atmega328old --port $(PORT) --upload --verbose 

fc: # Fligh Controller esp32
	arduino-cli compile ./ESP32FlightController/ESP32FlightController.ino --fqbn esp32:esp32:nodemcu-32s --port $(PORT) --upload --verbose 

imu:
	mkdir -p /tmp/IMUTest/
	cp ./ESP32FlightController/IMUTest.cpp /tmp/IMUTest/IMUTest.ino
	cp ./ESP32FlightController/filters/Madgwick.h /tmp/IMUTest/
	cp ./ESP32FlightController/filters/Madgwick.cpp /tmp/IMUTest/
	arduino-cli compile /tmp/IMUTest/IMUTest.ino --fqbn esp32:esp32:nodemcu-32s --port $(PORT) --upload --verbose 
	rm -r /tmp/IMUTest