loraRecv:
	arduino-cli compile ./ForTests/LoraRecieveTest/LoraRecieveTest.ino --fqbn arduino:avr:nano:cpu=atmega328old --port $(PORT) --upload --verbose 
