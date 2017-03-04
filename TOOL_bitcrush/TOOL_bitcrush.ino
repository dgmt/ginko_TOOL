/*
 * Author: Vladimir Riznic
 * dzedaj011@gmail.com
 * March 2017.
 * CV1 - audio in
 * CV2 - degrade
 * Clock - unused
 * Out - audio out
 */
#include <TOOL.h>

Tool tool;
void setup() {
  // put your setup code here, to run once:
}

void loop() {
  tool.dac(map(analogRead(1), 0, 1023, 0, 2000));
  delayMicroseconds(analogRead(2)*5);
}
