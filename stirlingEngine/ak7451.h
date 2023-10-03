/***************************************************************************
* File Name: ak7451.h
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

#ifndef AK7451_H
#define AK7451_H

#include <Arduino.h>
#include <SPI.h>

// Registers
typedef enum AK7451_Reg_e {
  REG_ANG     = 0x00, // 12 bit angle data
  REG_MAG     = 0x01, // Magnetic flux density strength roughly 1LSB/mT
  REG_CHMOD   = 0x02, // For mode state
  REG_ERRMON  = 0x03, // Error monitor
  REG_ZP      = 0x06, // For set up angle zero point
  REG_RDABZ   = 0x07, // For set up rotation direction and setup ABZ outputs
  REG_MLK     = 0x08, // For memory lock
  REG_EBDIS   = 0x09, // For set up abnormal detection disable
  REG_UVW     = 0x0A, // For set up UVW outputs
} AK7451_Reg;

// OPCODES
#define OPCODE_WRITE_EEPROM              0x01
#define OPCODE_READ_EEPROM               0x02
#define OPCODE_WRITE_REG                 0x03
#define OPCODE_READ_REG                  0x04
#define OPCODE_CHANGE_MODE               0x05
#define OPCODE_ANGLE_DATA_RENEW          0x08
#define OPCODE_READ_ANGLE                0x09

class AK7451
{
 public:
  AK7451();
  void begin(int8_t chipSelectPin, SPIClass &spiPort = SPI);
  uint16_t read_data(uint8_t opcode, uint8_t reg);
  float read_angle_data();

 private:
  uint8_t _cs;
  SPIClass *_spiPort;
  SPISettings _spiSettings;
  uint16_t raw_angle; // raw angle value
};

#endif // AK7451_H