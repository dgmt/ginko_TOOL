#ifndef TOOL_h
#define TOOL_h

#include "Arduino.h"
#include <SPI.h>
#include "DAC.h"
#include <PinChangeInterrupt.h>

class Tool {
  public:
    Tool();
    void dac(int dac_value);
    void set_trigger_input(void (*int_function));
  private:
    DAC _dac;
};

#endif
