![](Volumetric-LED-Matrix/pictures/display_calibration.svg)
This project was inspired by mitxelas tiny volumetric LED display and Nick electronics volumetric lamp project. 
The Matrix can display 3D animations on a spinning 16x16 LED matrix. The animations can be created and rendered and expoted as singular bitmaps in blender, then they must converted into suitable arrays by the bitmap converter tool. The converted arrays must then be included as header files to the volumetric led matrix disc code. 
Unfortunately, the matrix suffers from imbalance and thus heavy vibrations when spun. Thus, a 50 mm diameter and 52 mm tall piece of round steel was integrated into to the base as counterweight, aswell as a silicone pad on the bottom to further dampen vibrations. The power to the spinning top part is transported wireless from the base.
The choosen BLDC ESC was rather cheap and especially with a weak power supply, the motor cannot be started up.
The 5 V USB-C power supply must be strong. Not all power supplys work. Anker power banks and supplys have tested good for purpose of powering the volumetric led matrix.
Sometimes the animations or pictures show small glitches. This is a known issue still to be solved.
The filament for the wireless coil spool holders must be capable to withstand higher temperatures without bending itself. 

The screws used are M2. 

For disc to motor: 4 mm

For upper coil holder to PCB: 5 mm

For motor and lower coil holder: 8 mm

The motor stands on 4 mm standoffs, which are spaced with M2 washers to not pierce the motor coils.

All screws must be assembled with loctite for reliable long term use.


The used wireless module is wireless module 5V / 1A from AliExpress

The LED RGB ring is 16 LEDs WS2812 RGB module from AliExpress

The glass dome was bought on AliExpress

The 12 V mini step up module was bought on AliExpress

The ESC module was bought an AliExpress

The BLDC motor was bought on AliExpress

The 16x16 LED Matrix was assembled by JLCPCB PCBA service
The other circuit boards where assembled by hand.

The wireless coils where glued into the holders (make sure to remove the small piece of tape beforehand) with superglue. 
