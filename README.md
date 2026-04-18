# Volumetric LED Matrix
![](https://github.com/MoritzK42/Volumetric-LED-Matrix/blob/main/pictures/display_calibration.jpg)

## About the project
This project was inspired by mitxelas tiny volumetric LED display and Nick electronics volumetric lamp project.
It was created in the context of the elective project of the Hochschule für angewandte Wissenschaften (HAW) Hamburg, Germany.
The matrix can display 3D animations on a spinning 16x16 LED matrix, and is powered via USB-C. 

## Hardware
![more pictures can be found in the pictures folder](https://github.com/MoritzK42/Volumetric-LED-Matrix/blob/main/pictures/collage.png)
All used machine screws used are M2 with philips head bought on AliExpress. The used lengths are the following:

For disc to motor: 4 mm

For upper coil holder to PCB: 5 mm

For motor and lower coil holder: 8 mm

The motor stands on 4 mm standoffs, which are spaced with M2 washers to not pierce the motor coils.

All screws must be assembled with loctite for reliable long term use.

The used wireless module is 5V / 1A from AliExpress: https://de.aliexpress.com/item/1005006105734663.html?spm=a2g0o.order_list.order_list_main.82.53175c5fBHUX8V&gatewayAdapt=glo2deu

The LED RGB ring is 16 LEDs WS2812 RGB module from AliExpress: https://www.aliexpress.com/item/1005006579026411.html?spm=a2g0o.order_list.order_list_main.15.7f645c5fj35ANH

The glass dome was bought on AliExpress

The 12 V mini step up module was bought on AliExpress: https://de.aliexpress.com/item/1005005835656823.html?spm=a2g0o.order_detail.order_detail_item.9.743a6368CpXI0D&gatewayAdapt=glo2deu

The ESC module was bought an AliExpress: https://de.aliexpress.com/item/1005008034100991.html?spm=a2g0o.order_list.order_list_main.67.53175c5fBHUX8V&gatewayAdapt=glo2deu (8 A Version)

The BLDC motor was bought on AliExpress: https://de.aliexpress.com/item/1005006110056005.html?spm=a2g0o.order_list.order_list_main.57.53175c5fBHUX8V&gatewayAdapt=glo2deu

The 16x16 LED Matrix was assembled by JLCPCB PCBA service.

The other circuit boards where assembled by hand.

The wireless coils where glued into the holders (make sure to remove the small piece of tape beforehand) with superglue.
Unfortunately, the matrix suffers from imbalance and thus heavy vibrations when spun. Thus, a 50 mm diameter and 52 mm tall piece of round steel was integrated into to the base as counterweight, aswell as a silicone pad on the bottom to further dampen vibrations. The power to the spinning top part is transported wireless from the base.
The choosen BLDC ESC was rather cheap and especially with a weak power supply, the motor cannot be started up.
The 5 V USB-C power supply must be strong. Not all power supplys work. Anker power banks and supplys have tested good for purpose of powering the volumetric led matrix.
Sometimes the animations or pictures show small glitches. This is a known issue still to be solved.
The filament for the wireless coil spool holders must be capable to withstand higher temperatures without bending itself.
Also, a 1200 uF polymer capacitor was added between USB-C 5 V and GND to further improve stability. Two 22 uf 0805 ceramic capacitors have been added to the base ESP32 input in parallel to the 100 nf and 10 uf decoupling capacitors since there have been issues with the ESP32 crashing when sending wireless commands to the disc ESP32. This issue may have been fixed also by reducing the TX power level in software.

## Software and operation
The Bitmap converter is written in C# and used to convert the Blender generated bitmaps of a frame or animation into a single header file containing a 4-dimensional array. 

The dimensions of the array have the following function:
```c
﻿const uint8_t calibration[8][1][96][32] =
```

1. brightness compensation
2. frame of animation (1 for single 3D Image)
3. subframe (we decided to use 96 subframes per rotation in this project)
4. The single bytes making up the 256 bit LED matrix (8 x 32 = 256)

### Brightness compensation
Since the LEDs closer to the center of the cylindrical axis og the matrix have a lower distance to travel than the outer ones, they would appear to be brighter and thus overexpose the image. To mitigate this, the Bitmap converter automatically compensates the brightness and makes 8 additional frames which are compensated. This is at expense of MCU Flash, because now all frames take up eight times as much flash. But reducing the brightness in run by the MCU has proven to be too slow and would thus hinder a smooth display of the individual frames.

### Led Matrix Base
The LED matrix base containing the motor, buttons, wireless transmitter, 12 V step-up module, the RGB LED ring and position LED is controlled via an ESP32-C3. After plugging in the power, the ESP32 will flash the debugging LED twice and then fade the RGB ring through the colors. After pressing the left button, it will switch on the power to the wireless coil and ESC, which will in turn beep 3 times and turn on if initialized successful. The ESP32 on the disc will be powered and send an acknowledgement if it detects spinning. If not, the ESC initialisation has failed and the base ESP32 will power cycle the ESC to try again. Unfortunately this was the only way the reliably turn on the ESC, since 5 V USB-C supply seemed to be a poor choice and the ESC in general of poor quality. After successful turn on of motor and display, the animations can be cycled trough by pressing the center and right button. A long press will slowly spin the 3D object or animation. When the left button is pressed again, the motor stops and a 2D animation is displayed. The power to ESC and wireless transmitter is cut after 1 min in this state, to mitigate the issue of overheating coils and bent filament if left forgotten in this state.

### Led Matrix Disc
The LED matrix disc containing the LED matrix and LDR with comparator is also controlled via an ESP32-C3. The ESP32 references the start of a rotation via the comparator which compares the voltage from an LDR-resistor divider and potentiometer. Then the time of a rotation is sliced into 96 pieces where the subframes are displayed, altough, the timing is even more precise since there is also the aforementioned brightness compensation and also a settable percentage of on-time 96th time slot the create an effect of single LED dots. Thus, the code is highly optimized to deal with these very strict timing limitations and the hardware SPI-Bus is used to shift out the bits to the display drivers as fast as possible. The LEDs are not multiplexed, since this would, at these already super high refresh rates be almost impossible to deal with. Instead the TI 
The LED matrix disc containing the LED matrix and LDR with comparator is also controlled via an ESP32-C3. The ESP32 references the start of a rotation via the comparator which compares the voltage from an LDR-resistor divider and potentiometer. Then the time of a rotation is sliced into 96 pieces where the subframes are displayed, altough, the timing is even more precise since there is also the aforementioned brightness compensation and also a settable percentage of on-time 96th time slot the create an effect of single LED dots. Thus, the code is highly optimized to deal with these very strict timing limitations and the hardware SPI-Bus is used to shift out the bits to the display drivers as fast as possible. The LEDs are not multiplexed, since this would, at these already super high refresh rates be almost impossible to deal. Instead the 16 bit TI TLC59283DBQR serial shift register LED drivers ICs have been used to drive the LEDs. This choice has proven to be very effective, since the LED Matrix PCBA has performed absolutely flawlessly.

## Rendering animations in Blender
The 3D-Objects can be created in blender. They are then sliced and photographed in 1/96 increments of rotation. To create animations, the process must be repeated with all frames. The naming sceme which can be set in Blender when exporting has to be the following: "nameXX_YY", whereas the XX indicates the number of frame in the animation, and YY indicates the number of subframe in frame. A leading 0 must be added for numbers below 10. A python script in was written in Blender, to automate this process. Powerful hardware must be used to render the frames. They must be exported as singular bitmaps and then they must converted into suitable arrays by the bitmap converter tool. The converted arrays must then be included as header files to the volumetric led matrix disc code.

