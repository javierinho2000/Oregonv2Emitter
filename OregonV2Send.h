/*
 * Oregon Scientific v2.1 Emitter class
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

#include <arduino.h>
const unsigned long TIME = 512;
const unsigned long TWOTIME = TIME*2;


#define SEND_HIGH() digitalWrite(TX_PIN, HIGH)
#define SEND_LOW() digitalWrite(TX_PIN, LOW)
 

class OREGON {
public:
  OREGON(byte pin);
	void setBatteryLevel(byte level);
	void setTemperature(float temp);
	void setHumidity(byte hum);
	void calculateAndSetChecksum();
	void send(void);
private:
  // Buffer for Oregon message
  byte OregonMessageBuffer[9];
	byte TX_PIN;
	
	inline void sendZero(void);
	inline void sendOne(void);
	inline void sendQuarterMSB(const byte data);
	inline void sendQuarterLSB(const byte data);
	void sendData(byte *data, byte size);
	void sendOregon(byte *data, byte size);
	inline void sendPreamble(void);
	inline void sendPostamble(void);
	inline void sendSync(void);
	inline void setType(byte *data, byte* type);
	inline void setChannel(byte *data, byte channel);
	inline void setId(byte *data, byte id_);
	int Sum(byte count, const byte* data);
};
