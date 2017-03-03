#ifndef _DAC_H
#define _DAC_H

#include <SPI.h>
#include <Arduino.h>
#include <inttypes.h>

#ifndef _SPI_H_INCLUDED
#error Please include SPI.h before DAC.h!
#endif

// Microchip MCP4901/MCP4911/MCP4921/MCP4902/MCP4912/MCP4922 DAC driver
// Thomas Backman, 2012

class DAC {
  public:

  DAC(int _ldac_pin = -1);
  void setBuffer(boolean _buffer);
  void setPortWrite(boolean _port_write);
  boolean setGain(int _gain);
  boolean setSPIDivider(int _spi_divider);
  void shutdown(void);

  /* All of these call _output() internally */
  void output(unsigned short _out);

  void latch(void); // Actually change the output, if the LDAC pin isn't shorted to ground

  private:
    void _output(unsigned short _out);
    int ss_pin;
    int LDAC_pin;
    int bitwidth;
    boolean bufferVref;
    boolean gain2x; /* false -> 1x, true -> 2x */
    boolean port_write; /* use optimized port writes? won't work everywhere! */
    int spi_divider;
};

#endif
