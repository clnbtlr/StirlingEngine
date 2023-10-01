/* Stirling Engine Pressure and Volume measurement. 
CButler. Oct 2023.

Using Arduino Micro as MCB as sensors require 5V.
*/

#include <SPI.h>

void setup() {
  Serial.begin(115200);
  while(!Serial){
    delay(1);
  }
  Serial.println("***Stirling Engine Test***");
  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH); // disable Slave Select
  SPI.begin ();

}
const byte opcode = 0b1001; // AK7451 operation code for read the angle data
//byte opcode = 0b0101; // AK7451 operation code for change mode

byte buf[2]; // return data from AK7451

void loop() {
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE1));
  digitalWrite(SS, LOW); // enable Slave Select
  delay(10);
  SPI.transfer(0x00);
  SPI.transfer(opcode); 
  buf[0] = SPI.transfer(0);
  buf[1] = SPI.transfer(0);
  digitalWrite(SS, HIGH); // disable Slave Select
  delay(10);
  SPI.endTransaction();
  for (int i = 0; i < sizeof(buf); i++) {
    Serial.print(buf[i], BIN);
    Serial.print(" ");
  };
  Serial.println();
  delay(1000);
}
