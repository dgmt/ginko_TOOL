/*
 * Ginko Synthese TOOL module library
 * made by Vladimir Riznic
 * dzedaj011@gmail.com
 * https://github.com/dgmt

 * DAC code modified from Thomas Backman's MCP49xx library
 * https://github.com/exscape/electronics
 */

#include <SPI.h>
#include "DAC.h"

#define SS_PIN 10

DAC::DAC(int _LDAC_pin) : bufferVref(false), gain2x(false), port_write(false), spi_divider(SPI_CLOCK_DIV2)
{
  this->ss_pin = SS_PIN;
  this->LDAC_pin = _LDAC_pin;

 /*
  * MCP49x1 models are single DACs, while MCP49x2 are dual.
  * Apart from that, only the bit width differ between the models.
  */
  bitwidth = 12;

  pinMode(ss_pin, OUTPUT); // Ensure that SS is set to SPI master mode
  pinMode(LDAC_pin, OUTPUT);

  digitalWrite(ss_pin, HIGH); // Unselect the device
  digitalWrite(LDAC_pin, HIGH); // Un-latch the output

  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(spi_divider);
}

void DAC::setBuffer(boolean _buffer)
{
    this->bufferVref = _buffer;
}

void DAC::setPortWrite(boolean _port_write)
{
    this->port_write = _port_write;
}

// Sets the gain. These DACs support 1x and 2x gain.
// vout = x/2^n * gain * VREF, where x = the argument to out(), n = number of DAC bits
// Example: with 1x gain, set(100) on a 8-bit (256-step) DAC would give
// an output voltage of 100/256 * VREF, while a gain of 2x would give
// vout = 100/256 * VREF * 2
boolean DAC::setGain(int _gain) {
  if (_gain == 1) {
    gain2x = false;
    return true;
  }
  else if (_gain == 2) {
    gain2x = true;
    return true;
  }
  else
    return false; // DAC only supports 1x and 2x
}

// Sets the SPI clock frequency. The final frequency will be the
// main Arduino clock divided by the divider selected here.
boolean DAC::setSPIDivider(int _div) {
  switch (_div) {
    case SPI_CLOCK_DIV2:
    case SPI_CLOCK_DIV4:
    case SPI_CLOCK_DIV8:
    case SPI_CLOCK_DIV16:
    case SPI_CLOCK_DIV32:
    case SPI_CLOCK_DIV64:
    case SPI_CLOCK_DIV128:
      spi_divider = _div;
      SPI.setClockDivider(_div);
      return true;
    default:
      return false;
  }
}

// Shuts the DAC down. Shutdown current is about 1/50 (typical) of active mode current.
// My measurements say ~160-180 µA active (unloaded vout), ~3.5 µA shutdown.
// Time to settle on an output value increases from ~4.5 µs to ~10 µs, though (according to the datasheet).
void DAC::shutdown(void) {
  // Drive chip select low
  if (this->port_write)
    PORTB &= 0xfb; // Clear PORTB pin 2 = arduino pin 10
  else
    digitalWrite(ss_pin, LOW);

  // Sending all zeroes should work, too, but I'm unsure whether there might be a switching time
  // between buffer and gain modes, so we'll send them so that they have the same value once we
  // exit shutdown.
  unsigned short out = (bufferVref << 14) | ((!(gain2x)) << 13); // gain == 0 means 2x, so we need to invert it
  SPI.transfer((out & 0xff00) >> 8);
  SPI.transfer(out & 0xff);

  // Return chip select to high
  if (this->port_write)
    PORTB |= (1 << 2); // set PORTB pin 2 = arduino pin 10
  else
    digitalWrite(ss_pin, HIGH);
}

// Private function.
// Called by the output* set of functions.
void DAC::_output(unsigned short data) {
  int chan = 0;
  // Truncate the unused bits to fit the 8/10/12 bits the DAC accepts
  if (this->bitwidth == 12)
    data &= 0xfff;
  else if (this->bitwidth == 10)
    data &= 0x3ff;
  else if (this->bitwidth == 8)
    data &= 0xff;

  // Drive chip select low
  if (this->port_write)
    PORTB &= 0xfb; // Clear PORTB pin 2 = arduino pin 10
  else
    digitalWrite(ss_pin, LOW);

  // bit 15: 0 for DAC A, 1 for DAC B. (Always 0 for MCP49x1.)
  // bit 14: buffer VREF?
  // bit 13: gain bit; 0 for 1x gain, 1 for 2x (thus we NOT the variable)
  // bit 12: shutdown bit. 1 for active operation
  // bits 11 through 0: data
  uint16_t out = (chan << 15) | (this->bufferVref << 14) | ((!this->gain2x) << 13) | (1 << 12) | (data << (12 - this->bitwidth));

  // Send the command and data bits
  SPI.transfer((out & 0xff00) >> 8);
  SPI.transfer(out & 0xff);

  // Return chip select to high
  if (this->port_write)
    PORTB |= (1 << 2); // set PORTB pin 2 = arduino pin 10
  else
    digitalWrite(ss_pin, HIGH);
}

// For MCP49x1
void DAC::output(unsigned short data) {
  this->_output(data);
}

// These DACs have a function where you can change multiple DACs at the same
// time: you call output() "sequentially", one DAC at a time, and *then*,
// when they've all received the output data, pull the LDAC pin low on
// all DACs at once. This function pulls the LDAC pin low for long enough
// for the DAC(s) to change the output.
// If this function is undesired, you can simply tie the LDAC pin to ground.
// When tied to ground, you need *NOT* call this function!
void DAC::latch(void) {
  // The datasheet says CS must return high for 40+ ns before this function
  // is called: no problems, that'd be taken care of automatically, as one
  // clock cycle at 16 MHz is longer... and there'll be a delay of multiple.

  if (LDAC_pin < 0)
    return;

  // We then need to hold LDAC low for at least 100 ns, i.e ~2 clock cycles.

  if (this->port_write) {
    // This gives ~180 ns (three clock cycles, most of which is spent low) of
    // low time on a Uno R3 (16 MHz), measured on a scope to make sure
    PORTD &= ~(1 << 7); // Uno: digital pin 7; Mega: digital pin 38
    asm volatile("nop");
    PORTD |= (1 << 7);
  }
  else {
    // This takes far, FAR longer than the above despite no NOP; digitalWrite
    // is SLOW! For comparison: the above takes 180 ns, this takes... 3.8 us,
    // or 3800 ns, 21 times as long - WITHOUT having a delay in there!
    digitalWrite(LDAC_pin, LOW);
    digitalWrite(LDAC_pin, HIGH);
  }
}
