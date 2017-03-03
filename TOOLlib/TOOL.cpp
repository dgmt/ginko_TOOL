#include "Arduino.h"
#include "TOOL.h"

#define CLOCK_PIN 12

Tool::Tool() {
  _dac.setSPIDivider(SPI_CLOCK_DIV16);
  _dac.setPortWrite(true);
}

void Tool::dac(int dac_value) {
  _dac.output(dac_value);
}

void Tool::set_trigger_input(void (*int_function)){
  pinMode(CLOCK_PIN, INPUT);
  attachPCINT(digitalPinToPCINT(CLOCK_PIN), int_function, FALLING);
}
