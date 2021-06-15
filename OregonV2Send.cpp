/*
 * Oregon Scientific v2.1 Emitter class
 * 
 * Provides code to emulate the emitter part of the protocol.
 * 
 * Based on :
 *    connectingStuff, Oregon Scientific v2.1 Emitter
 *    http://connectingstuff.net/encodage-protocoles-oregon-scientific-sur-arduino/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "OregonV2Send.h"
#include <arduino.h>

OREGON::OREGON(byte pin)
{
	TX_PIN = pin;
	pinMode(TX_PIN, OUTPUT);
	digitalWrite(TX_PIN, LOW);  
  // Create the Oregon message for a temperature/humidity sensor (THGR2228N)
  byte ID[] = {0x1A,0x2D}; 
	setType(OregonMessageBuffer, ID);
  setChannel(OregonMessageBuffer, 0x20);
  setId(OregonMessageBuffer, 0xBB);
}


/**
 * \brief    Send logical "0" over RF
 * \details  azero bit be represented by an off-to-on transition
 * \         of the RF signal at the middle of a clock period.
 * \         Remenber, the Oregon v2.1 protocol add an inverted bit first 
 */
 
inline void OREGON::sendZero(void) 
{
  digitalWrite(TX_PIN, HIGH);
  delayMicroseconds(TIME);
  digitalWrite(TX_PIN, LOW);
  delayMicroseconds(TWOTIME);
  digitalWrite(TX_PIN, HIGH);
  delayMicroseconds(TIME);
}

/**
 * \brief    Send logical "1" over RF
 * \details  a one bit be represented by an on-to-off transition
 * \         of the RF signal at the middle of a clock period.
 * \         Remenber, the Oregon v2.1 protocol add an inverted bit first 
 */
inline void OREGON::sendOne(void) 
{
   digitalWrite(TX_PIN, LOW);
   delayMicroseconds(TIME);
   digitalWrite(TX_PIN, HIGH);
   delayMicroseconds(TWOTIME);
   digitalWrite(TX_PIN, LOW);
   delayMicroseconds(TIME);
}

/**
* Send a bits quarter (4 bits = MSB from 8 bits value) over RF
*
* @param data Source data to process and sent
*/
 
/**
 * \brief    Send a bits quarter (4 bits = MSB from 8 bits value) over RF
 * \param    data   Data to send
 */
inline void OREGON::sendQuarterMSB(const byte data) 
{
  (bitRead(data, 4)) ? sendOne() : sendZero();
  (bitRead(data, 5)) ? sendOne() : sendZero();
  (bitRead(data, 6)) ? sendOne() : sendZero();
  (bitRead(data, 7)) ? sendOne() : sendZero();
}
 
/**
 * \brief    Send a bits quarter (4 bits = LSB from 8 bits value) over RF
 * \param    data   Data to send
 */
inline void OREGON::sendQuarterLSB(const byte data) 
{
  (bitRead(data, 0)) ? sendOne() : sendZero();
  (bitRead(data, 1)) ? sendOne() : sendZero();
  (bitRead(data, 2)) ? sendOne() : sendZero();
  (bitRead(data, 3)) ? sendOne() : sendZero();
}

/**
 * \brief    Send a buffer over RF
 * \param    data   Data to send
 * \param    size   size of data to send
 */
void OREGON::sendData(byte *data, byte size)
{
  for(byte i = 0; i < size; ++i)
  {
    sendQuarterLSB(data[i]);
    sendQuarterMSB(data[i]);
  }
}

/**
 * \brief    Send an Oregon message
 * \param    data   The Oregon message
 */
void OREGON::sendOregon(byte *data, byte size)
{
    sendPreamble();
    //sendSync();
    sendData(data, size);
    sendPostamble();
}

/**
 * \brief    Send preamble
 * \details  The preamble consists of 16 "1" bits
 */
inline void OREGON::sendPreamble(void)
{
  byte PREAMBLE[]={0xFF,0xFF};
  sendData(PREAMBLE, 2);
}

/**
 * \brief    Send postamble
 * \details  The postamble consists of 8 "0" bits
 */
inline void OREGON::sendPostamble(void)
{

  byte POSTAMBLE[]={0x00};
  sendData(POSTAMBLE, 1);  
}

/**
 * \brief    Send sync nibble
 * \details  The sync is 0xA. It is not use in this version since the sync nibble
 * \         is include in the Oregon message to send.
 */
inline void OREGON::sendSync(void)
{
  sendQuarterLSB(0xA);
}

/**
 * \brief    Set the sensor type
 * \param    data       Oregon message
 * \param    type       Sensor type
 */
inline void OREGON::setType(byte *data, byte* type) 
{
  data[0] = type[0];
  data[1] = type[1];
}

/**
 * \brief    Set the sensor channel
 * \param    data       Oregon message
 * \param    channel    Sensor channel (0x10, 0x20, 0x30)
 */
inline void OREGON::setChannel(byte *data, byte channel) 
{
    data[2] = channel;
}

/**
 * \brief    Set the sensor ID
 * \param    data       Oregon message
 * \param    ID         Sensor unique ID
 */
inline void OREGON::setId(byte *data, byte id_) 
{
  data[3] = id_;
}

/**
 * \brief    Set the sensor battery level
 * \param    data       Oregon message
 * \param    level      Battery level (0 = low, 1 = high)
 */
void OREGON::setBatteryLevel(byte level)
{
  if(!level) OregonMessageBuffer[4] = 0x0C;
  else OregonMessageBuffer[4] = 0x00;
}

/**
 * \brief    Set the sensor temperature
 * \param    data       Oregon message
 * \param    temp       the temperature
 */
void OREGON::setTemperature(float temp) 
{
  // Set temperature sign
  if(temp < 0)
  {
    OregonMessageBuffer[6] = 0x08;
    temp *= -1;  
  }
  else
  {
    OregonMessageBuffer[6] = 0x00;
  }
 
  // Determine decimal and float part
  int tempInt = (int)temp;
  int td = (int)(tempInt / 10);
  int tf = (int)round((float)((float)tempInt/10 - (float)td) * 10);
 
  int tempFloat =  (int)round((float)(temp - (float)tempInt) * 10);
 
  // Set temperature decimal part
  OregonMessageBuffer[5] = (td << 4);
  OregonMessageBuffer[5] |= tf;
 
  // Set temperature float part
  OregonMessageBuffer[4] |= (tempFloat << 4);
}

/**
 * \brief    Set the sensor humidity
 * \param    data       Oregon message
 * \param    hum        the humidity
 */
void OREGON::setHumidity(byte hum)
{
    OregonMessageBuffer[7] = (hum/10);
    OregonMessageBuffer[6] |= (hum - OregonMessageBuffer[7]*10) << 4;
}

/**
 * \brief    Sum data for checksum
 * \param    count      number of bit to sum
 * \param    data       Oregon message
 */
int OREGON::Sum(byte count, const byte* data)
{
  int s = 0;
 
  for(byte i = 0; i<count;i++)
  {
    s += (data[i]&0xF0) >> 4;
    s += (data[i]&0xF);
  }
 
  if(int(count) != count)
    s += (data[count]&0xF0) >> 4;
 
  return s;
}

/**
 * \brief    Calculate checksum
 * \param    data       Oregon message
 */
void OREGON::calculateAndSetChecksum()
{

    OregonMessageBuffer[8] = ((Sum(8, OregonMessageBuffer) - 0xa) & 0xFF);
}

/**
 * \brief    Send all data, calls sendOregon()
 * \param    void
 */
void OREGON::send(void)
{
  // Calculate checksum
  calculateAndSetChecksum();
    // Show the Oregon Message
  for (byte i = 0; i < sizeof(OregonMessageBuffer); ++i)   {     Serial.print(OregonMessageBuffer[i] >> 4, HEX);
    Serial.print(OregonMessageBuffer[i] & 0x0F, HEX);
  }
  // Send the Message over RF
  sendOregon(OregonMessageBuffer, sizeof(OregonMessageBuffer));
  // Send a "pause"
  digitalWrite(TX_PIN, LOW);
  delayMicroseconds(TWOTIME*8);
  // Send a copie of the first message. The v2.1 protocol send the
  // message two time 
  sendOregon(OregonMessageBuffer, sizeof(OregonMessageBuffer));
  // Wait for 30 seconds before send a new message 
  digitalWrite(TX_PIN, LOW);
}
