/* Test of library for AK7451 IC on Mikroe Angle 3 Click breakout board.
03/10/2023.
*/
#include "ak7451.h"

AK7451  ak7451;

void setup() {
  Serial.begin(115200);
  while(!Serial){
    delay(1);
  }
  Serial.println("***Reset***");
  ak7451.begin(SS);
}

void loop() {
  float angle = ak7451.read_angle_data();
  Serial.print(angle);
  bool err_pin = ak7451.read_error_pin();
  Serial.print(", ");Serial.println(err_pin);
  delay(1000);
}
