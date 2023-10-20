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
  A5 <-> VOUT
*/

#include <SPI.h>
#include "ak7451.h"
AK7451  ak7451;

int r = 4; // offset distance between axis and crank [mm]
int L = 40; // crank arm length [mm]
float A = PI/4.0*sq(16.0); // piston cross sectional area [mm2]
float zeroPos = 123.0;//237.0; // angle reading corresponding to highest point of piston [deg]
int pressurePin = A5; // analog input pin for pressure sensor

void setup() {
  Serial.begin(19200);
  while(!Serial){ // wait for serial port
    delay(1);
  }
  Serial.println("***Reset***");
  ak7451.begin(SS); // SS pin on Arduino Micro but can be any other Digital I/O Pin
}

void loop() {
  float supply = readVcc()/1000.0; // read Vcc supply
  float angle = ak7451.readAngle(); // read angle from AK7451 sensor
  angle = fmodf(360-angle+zeroPos,360.0); // removes zeroPos from measured angle and wraps to 360 deg using modulo function
  float pressure = analogRead(pressurePin)*supply/1024.0; // read bits from pressure sensor and convert to voltage 
  pressure = (pressure/supply-0.5)/0.2; // convert V to kPa (see MPXV7002 datasheet p.6 for transfer function)
  float h = r*(cos(deg2rad(angle))-L/r*(sqrt(1-(r/L*sq(sin(deg2rad(angle)))))-1)); // change in piston height (see Lu paper, eq.2)
  float volume = A*h/1000.0; // volume change [cm3]
  //Serial.print("angle:"); Serial.print(angle); Serial.print(",");// print angle to Serial Monitor
  Serial.print("pressure:");Serial.print(pressure,3); Serial.print(",");// print pressure to Serial Monitor
  Serial.print("volume:");Serial.println(volume,3); // print volume to Serial Monitor
  delay(50); // delay between measurements (Lu paper used 200 samples/sec)
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

float deg2rad(float angle){ // convert degrees to radians
  float rad = angle*PI/180.0;
  return(rad);
}