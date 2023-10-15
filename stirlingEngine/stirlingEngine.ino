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
  float supply = readVcc() / 1000.0; // read Vcc supply
  float angle = ak7451.readAngle(); // read angle from AK7451 sensor
  float pressure = analogRead(pressurePin)*supply/1024.0; // read bits from MPXV7025 sensor and convert to voltage 
  pressure = (pressure/supply-0.5)/0.018; // convert V to kPa (see datasheet p.5 for transfer function)
  
  Serial.print(angle); // print angle to Serial Monitor
  Serial.print(","); // print angle to Serial Monitor
  Serial.println(pressure);
  delay(1000); // delay 1 second between measurements 
}

long readVcc() { // Read supply voltage to improve ADC precision from https://github.com/SensorsIot/ADC_Test.git
  long result;
  // Read 1.1V reference against AVcc
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
  ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA, ADSC));
  result = ADCL;
  result |= ADCH << 8;
  result = 1126400L / result; // Calculate Vcc (in mV); 1126400 = 1.1*1024*1000
  return result;
}
