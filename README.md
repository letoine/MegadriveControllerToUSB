MegadriveControllerToUSB
========================

Simple adapter that transforms a Megadrive/Genesis controller into a USB controller.

Megadrive controller pinout
===========================

pin | name         | comment
----|--------------|--------
 1  | data0        | 
 2  | data1        |
 3  | data2        |
 4  | data3        | 
 5  | +5V          | 
 6  | data4        | 
 7  | Select clock | 
 8  | GND          | 
 9  | data5        | 


Select clock |  data0   |  data1   |  data2   | data3 |    data4     |  data5 
-------------|----------|----------|----------|-------|--------------|---------
 High        | Up       | Down     | Left     | Right | C button     | B button
 Low         | Up       | Down     | 0        | 0     | Start button | A button
 High        | Up       | Down     | Left     | Right | C button     | B button
 Low         | 0        | 0        | 0        | 0     | Start button | A button
 High        | Z button | Y button | X button | Mode  | C button     | B button
 Low         | 1        | 1        | 1        | 1     | Start button | B button
