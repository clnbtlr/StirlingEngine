/* Stirling Engine Pressure and Volume measurement. 
CButler. Oct 2023.

Using Arduino Micro as MCB as sensors require 5V.
Pinout:
  AK7451:
  5V <-> 5V
  GND <-> GND
  SCK <-> SCK
  MI <-> SDO
  MO <-> SDI
  SS <-> CS

  MPXV7025GC6U:
  5V <-> VS
  GND <-> GND
  A6 <-> VOUT
*/

#include <SPI.h>
#include "ak7451.h"
AK7451  ak7451;

int pressurePin = A5; // analog input pin for the pressure sensor

void setup() {
  Serial.begin(115200);
  while(!Serial){ // wait for serial port
    delay(1);
  }
  Serial.println("***Reset***");
  ak7451.begin(SS); // SS pin on Arduino Micro but can be any other Digital I/O Pin
}

void loop() {
  float angle = ak7451.readAngle(); // read angle from AK7451 sensor
  float pressure = analogRead(pressurePin)*5.0/1024; // read bits from MPXV7025 sensor and convert to voltage 
  //pressure = (pressure/5.0-0.5)/0.018; // convert V to kPa (see datasheet p.5 for transfer function)
  
  Serial.print(angle); // print angle to Serial Monitor
  Serial.print(","); // print angle to Serial Monitor
  Serial.println(pressure);
  delay(1000); // delay 1 second between measurements 
}
