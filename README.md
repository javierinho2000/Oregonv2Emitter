# Oregonv2Emitter
Provides class for Oregon Scientific V2.1 very basic Emitter

It's basically the same code as in http://connectingstuff.net/encodage-protocoles-oregon-scientific-sur-arduino/ but in a c++ class for easier reuse. 

I needed to emulate an Oregon Scientific weather station (433.92 Mhz) and the above code did the trick.

Code is tested on ESP32, other similar boards should work too.

I'm receiving the signal on a RFLINK (433.92Mhz) board from Nodo, it works like a charm.
