loraRecv:
	arduino-cli compile ./ForTests/LoraRecieveTest/LoraRecieveTest.ino --fqbn arduino:avr:nano:cpu=atmega328old --port $(PORT) --upload --verbose 

fc: # Fligh Controller esp32
	arduino-cli compile ./ESP32FlightController/ESP32FlightController.ino --fqbn esp32:esp32:nodemcu-32s --port $(PORT) --upload --verbose 