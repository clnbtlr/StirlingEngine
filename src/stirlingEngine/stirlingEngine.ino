/* Stirling Engine Pressure and Volume measurement. 
CButler. Oct 2023.

Using Arduino Micro as MCB as sensors require 5V.
*/

#include <SPI.h>

#define CS 2

void setup() {
  Serial.begin(115200);
  while(!Serial){
    delay(1);
  }
  Serial.println("***Reset***");
  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH); // disable Chip Select
  SPI.begin();

}
//const byte opcode = 0b1001; // AK7451 operation code for read the angle data
//byte opcode = 0b0101; // AK7451 operation code for change mode

void loop() {
  uint8_t reg = 0x02; // register R_ANG
  uint8_t opcode = 0x05; // opcode READ_ANGLE

  // uint8_t tx_buf[1] = {0};
  // uint8_t rx_buf[2] = {0,0};
  // uint16_t raw_data = 0;
  // reg <<= 1; // register address is 7 bits
  // tx_buf[ 0 ] = ( opcode << 4 ) | ( reg >> 4);
  // Serial.println(tx_buf[0], BIN);

  // SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3));
  // digitalWrite(CS, LOW); // enable Chip Select
  // delay(10);
  // SPI.transfer(tx_buf,1); 
  // rx_buf[0] = SPI.transfer(0);
  // rx_buf[1] = SPI.transfer(0);
  // delay(10);
  // digitalWrite(CS, HIGH); // disable Chip Select
  // SPI.endTransaction();

  // raw_data = rx_buf[0];
  // raw_data <<=8;
  // raw_data |= rx_buf[1];
  // raw_data &= 0x0FFF;
  // Serial.print((uint16_t)raw_data, BIN);
  // float angle = (360*(float)raw_data)/4096;
  // Serial.print(", ");Serial.println(angle);
  // delay(1000);

  // Change Mode
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3));
  digitalWrite(CS, LOW); // enable Chip Select
  SPI.transfer16(0x050F);
  digitalWrite(CS, HIGH);
  SPI.endTransaction();
}
