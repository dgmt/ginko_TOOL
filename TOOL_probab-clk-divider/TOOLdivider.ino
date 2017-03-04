/*
 * Author: Vladimir Riznic
 * dzedaj011@gmail.com
 * March 2017.
 * CV1 - division
 * CV2 - probability
 * Clock - Needed
 * Out - Trigger
 * Trigger tested on Basimilus Iteritas and Maths,
 * triggers both, if you encounter a module that doesn't
 * trigger, increase the output value or lenghten the delay
 */
#include <TOOL.h>

Tool tool;

int counter = 0;
int divisions[] = {1, 2, 3, 4, 5, 7, 8, 12, 13, 14, 16, 32};
int div_length = sizeof(divisions)/sizeof(int);

void setup() {
  Serial.begin(9600);
  Serial.write("Setting up!\n");
  tool.set_trigger_input(tick);
}

void tick(){
  counter++;
  int ind = map(analogRead(1), 0, 1024, 0, div_length);
  int division = divisions[ind];
  if (counter == division) {
    counter = 0;
    bool shouldbang = false;
    int rnd = random(0, 1023);
    if (analogRead(2) >= rnd){
      tool.dac(1200);
      Serial.print("\nBANG!\n");
      delayMicroseconds(100);
      tool.dac(0); 
    }
  } else {
    if (counter > division) {
      counter = 0;
    }
    Serial.print("Nope!");
  }
}

void loop() {
  Serial.write("LOOP\n");
  delay(1000);
}
