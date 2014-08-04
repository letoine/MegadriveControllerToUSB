MegadriveControllerToUSB
========================

Simple adapter that transforms a Megadrive/Genesis controller into a USB controller.

Megadrive controller pinout
===========================

pin | name                  | comment
----|-----------------------|--------
 1  | Up/Z button           | 
 2  | Down/Y button         |
 3  | Left/X button         |
 4  | Right/Left/Mode       | 
 5  | +5V                   | 
 6  | C button/Start button | 
 7  | Select clock          | 
 8  | GND                   | 
 9  | B button/A button     | 


Select clock |  pin 1   |  pin 2   |  pin 3   | pin 4 |    pin 6     | pin 9 
-------------|----------|----------|----------|-------|--------------|------
 High        | Up       | Down     | Left     | Right | C button     | B button
 Low         | Up       | Down     | 0        | 0     | Start button | A button
 High        | Up       | Down     | Left     | Left  | C button     | B button
 Low         | 0        | 0        | 0        | 0     | Start button | A button
 High        | Z button | Y button | X button | Mode  | C button     | B button
 Low         | 1        | 1        | 1        | 1     | Start button | B button