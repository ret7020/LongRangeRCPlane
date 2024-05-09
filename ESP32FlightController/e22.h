// Functions for Ebyte modules: 433T30D and 900T3D
#ifndef LORA_COMMANDS_H

#define LORA_COMMANDS_H

void e22SetMode();
void e22ReadConfig(int *configRegs, int count, HardwareSerial &e22Serial);
void e22ParseConfig(int *configRegs, int count, HardwareSerial &e22Serial);
void e22SetRegister(byte reg, byte value, HardwareSerial &e22Serial);
void e22PrintConfig(int *configRegs, HardwareSerial &outputSerial);
void e22Transmit();

// char loraReadConfigRegs[] = { 0xC1, 0x00, 0x06 };

#endif

