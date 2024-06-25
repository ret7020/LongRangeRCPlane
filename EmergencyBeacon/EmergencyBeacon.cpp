#include "si5351.h"
#include "Wire.h"

#define RF_FREQ_0 136000.0
#define RF_FREQ_1 136500.0
#define RF_BIT_TIME 100

Si5351 si5351;

void setup()
{
  bool i2c_found;

  pinMode(LED_BUILTIN,OUTPUT);
  Serial.begin(115200);

  i2c_found = si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);
  if(!i2c_found)
  {
    Serial.println("si5351 not found; check wiring");
    while(1)
    {
      digitalWrite(LED_BUILTIN,HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN,LOW);
      delay(100);
    }
  }
  
  si5351.update_status();
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA);
  si5351.set_correction(80000, SI5351_PLL_INPUT_XO);
  si5351.set_freq(RF_FREQ_0 * 100000ULL, SI5351_CLK0);
  si5351.output_enable(SI5351_CLK0, 0);
}

void loop()
{
  si5351.output_enable(SI5351_CLK0, 1);
  delay(500);
  si5351.output_enable(SI5351_CLK0, 0);
  delay(1000);
}
