/*
 * Author: Vladimir Riznic
 * dzedaj011@gmail.com
 * March 2017.
 * CV1 - audio in
 * CV2 - freee speed
 * Clock - init freeze
 * Out - audio out
 */
#include <TOOL.h>

#define TRIGGER 12
#define MAX_BUFFER_LENGTH 512

Tool tool;

int audio_buffer[MAX_BUFFER_LENGTH];
int buffer_index = 0;

void setup() {
  Serial.begin(9600);
  pinMode(TRIGGER, INPUT);
  tool.set_trigger_input(tick);
}

void loop() {
  if (buffer_index == MAX_BUFFER_LENGTH){
    buffer_index = 0;
  }
  int sample = map(analogRead(1), 0, 1023, 0, 2000);
  audio_buffer[buffer_index] = sample;
  buffer_index++;
  tool.dac(sample);
  delayMicroseconds(10);
}

void tick(){
  int is_trig = digitalRead(TRIGGER);
  while (is_trig == LOW){
    int freeze_speed = analogRead(2);
    for (int i = 0; i < MAX_BUFFER_LENGTH; i++){
      tool.dac(audio_buffer[i]);
      delayMicroseconds(freeze_speed);
    }
    is_trig = digitalRead(TRIGGER);
  }
}

