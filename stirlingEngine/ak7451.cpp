/***************************************************************************
* File Name: ak7451.cpp
* Processor/Platform: Arduino Uno R3 (tested)
* Development Environment: Arduino 2.2.1
*
* Designed for use with with AK7451 magnetic angle sensor IC
* breakout boards: Mikroe Angle 3 Click
*
* Copyright © 2023
* SOFTWARE LICENSE AGREEMENT: This code is released under the MIT License.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
* **************************************************************************/

#include "AK7451.h"

AK7451::AK7451() :
  _spiSettings(2000000, MSBFIRST, SPI_MODE2)
{
}

void AK7451::begin(int8_t chipSelectPin, SPIClass &spiPort)
{
  // Function to initialize channel, load private variables
  _cs = chipSelectPin;
  _spiPort = &spiPort;

  pinMode(_cs, OUTPUT);
  digitalWrite(_cs, HIGH); // immediately pull CS pin high to avoid conflicts on SPI bus
  _spiPort->begin();
}

uint16_t AK7451::read_data(uint8_t opcode, uint8_t reg)
{
  uint8_t tx_buf = 0;
  uint8_t rx_buf[2] = {0,0};
  uint16_t raw_data = 0;

  reg <<= 1; // register address is 7 bits
  tx_buf = ( opcode << 4 ) | ( reg >> 4 );

  _spiPort->beginTransaction(_spiSettings);  
  digitalWrite(_cs, LOW);           // set pin low to start talking to IC

  _spiPort->transfer(tx_buf);       // transfer command
  rx_buf[0] = _spiPort->transfer(0);   // read first byte of data from IC
  rx_buf[1] = _spiPort->transfer(0);   // read second byte of data from IC

  digitalWrite(_cs, HIGH);          // set pin high to end SPI session
  _spiPort->endTransaction();

  raw_data = rx_buf[0];
  raw_data <<=8;
  raw_data |= rx_buf[1];

  return raw_data;
}

float AK7451::read_angle_data()
{
  raw_angle = read_data(OPCODE_READ_ANGLE, REG_ANG);
  raw_angle &= 0x0FFF; // angle data is 12 bits
  float angle = (360*(float)raw_angle)/4096;

  return angle;
}