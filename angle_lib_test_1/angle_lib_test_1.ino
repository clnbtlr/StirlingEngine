/* Test of library for AK7451 IC on Mikroe Angle 3 Click breakout board.
03/10/2023.
*/
#include "ak7451.h"

#define CS 10 // chip select pin
AK7451  ak7451;

void setup() {
  Serial.begin(115200);
  while(!Serial){
    delay(1);
  }
  Serial.println("***Reset***");
  ak7451.begin(CS);
}

void loop() {
  float angle = ak7451.read_angle_data();
  Serial.println(angle);
  delay(1000);
}
