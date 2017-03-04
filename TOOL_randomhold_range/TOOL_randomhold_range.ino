/*
 * Author: Vladimir Riznic
 * dzedaj011@gmail.com
 * March 2017.
 */
#include <TOOL.h>

#define MAXIMUM 2000

Tool tool;

void setup() {
  Serial.begin(9600);
  Serial.write("BEGINNING!\n");
  /*
   * set_trigger_input is setting the interrupt on receiving trigger
   * to CLOCK jack, and refering to the tick function which will
   * handle the interrupt
   */
  tool.set_trigger_input(tick);
}

void loop() {
  // not much to do here, S&H works on trigger
  Serial.write("LOOP!\n");
}

/*
 * Handler function should receive no arguments, and also, not return
 * a value
 */
void tick(){
  Serial.write("Inside the interrupt");
  int cv1 = analogRead(1);
  int cv2 = analogRead(2);
  int out_value = 0;
  if (cv2 >= cv1){
    out_value = random(cv1, cv2);
  } else {
    out_value = random(cv2, cv1);
  }
  int scaled_out = map(out_value, 0, 1023, 0, MAXIMUM);
  // write out the generated value
  tool.dac(scaled_out);
}

