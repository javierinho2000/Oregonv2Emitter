/*
 * Example use of Oregon Scientific v2.1 Emitter class
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

#include "./OregonV2Send.h"
 
const byte TXpin = 25;


// Initialize Oregon
OREGON oregon(TXpin);
 
void setup()
{
  
  Serial.begin(115200);
  Serial.println("\n[Oregon V2.1 encoder]");
}
 
void loop()
{
  oregon.setBatteryLevel(1); // 0 : low, 1 : high
  oregon.setTemperature(11.2); // temperature, you'd normally get this from a sensor
  oregon.setHumidity(52);// humidity, you'd normally get this from a sensor

  oregon.send();
 
  delay(30000);
}
