/*
 * Author: Vladimir Riznic
 * dzedaj011@gmail.com
 * March 2017.
 * CV1 - Select voltage to output
 */
#include <TOOL.h>

/* 
 * define maximum output, since tool is 0-10V for now, we want to  
 * protect the module receiving its CV out.
 */
#define MAXIMUM 2000
/* 
 * Initialize the library. Do this here, so it has a global scope. 
 * This way you can access it from both setup() and loop() functions.
 */
Tool tool;

void setup() {
  /*
   * It is a good idea to pepper your code while developing with
   * Serial.write() or Serial.print() so you can more effectively
   * debug the behaviour using serial monitor integrated into 
   * Arduino IDE.
   */
  Serial.begin(9600);
  Serial.write("Setting up!\n");
}

void loop() {
  // read pot1 value
  int cv1 = analogRead(1);
  // read pot2 value, we wont be using it here, but here is 
  // how it is done
  int cv2 = analogRead(2);
  /*
   * map function scales values proportionally, since ADC in the 
   * Arduino is 10bit, and our DAC is 12bit, we need to approximate
   * and compensate.
   */
  int scaled_cv1 = map(cv1, 0, 1023, 0, MAXIMUM);
  /*
   * now fun part - writing CV out
   * this is where we use library.
   */
  tool.dac(scaled_cv1);
  Serial.print(scaled_cv1);
  Serial.write(" written to the output.\n");
} 
