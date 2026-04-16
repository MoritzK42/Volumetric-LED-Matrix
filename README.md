![](https://github.com/MoritzK42/Volumetric-LED-Matrix/blob/main/pictures/display_calibration.jpg)
This project was inspired by mitxelas tiny volumetric LED display and Nick electronics volumetric lamp project. 
The Matrix can display 3D animations on a spinning 16x16 LED matrix. The animations can be created and rendered and expoted as singular bitmaps in blender, then they must converted into suitable arrays by the bitmap converter tool. The converted arrays must then be included as header files to the volumetric led matrix disc code. 
Unfortunately, the matrix suffers from imbalance and thus heavy vibrations when spun. Thus, a 50 mm diameter and 52 mm tall piece of round steel was integrated into to the base as counterweight, aswell as a silicone pad on the bottom to further dampen vibrations. The power to the spinning top part is transported wireless from the base.
The choosen BLDC ESC was rather cheap and especially with a weak power supply, the motor cannot be started up.
The 5 V USB-C power supply must be strong. Not all power supplys work. Anker power banks and supplys have tested good for purpose of powering the volumetric led matrix.
Sometimes the animations or pictures show small glitches. This is a known issue still to be solved.
The filament for the wireless coil spool holders must be capable to withstand higher temperatures without bending itself.
Also, a 1200 uF polymer capacitor was added between USB-C 5 V and GND to further improve stability.

The screws used are M2. 

For disc to motor: 4 mm

For upper coil holder to PCB: 5 mm

For motor and lower coil holder: 8 mm

The motor stands on 4 mm standoffs, which are spaced with M2 washers to not pierce the motor coils.

All screws must be assembled with loctite for reliable long term use.


The used wireless module is wireless module 5V / 1A from AliExpress: https://de.aliexpress.com/item/1005006105734663.html?spm=a2g0o.order_list.order_list_main.82.53175c5fBHUX8V&gatewayAdapt=glo2deu

The LED RGB ring is 16 LEDs WS2812 RGB module from AliExpress: https://de.aliexpress.com/item/1005006579026411.html?spm=a2g0o.productlist.main.55.20c8rBMUrBMUBh&algo_pvid=e281a54f-07bf-4cd3-a832-4d5de2bb4440&algo_exp_id=e281a54f-07bf-4cd3-a832-4d5de2bb4440-52&pdp_ext_f=%7B%22order%22%3A%2244%22%2C%22eval%22%3A%221%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21EUR%213.18%211.59%21%21%2124.98%2112.49%21%40210390b817763413623183330e4f40%2112000037719757056%21sea%21DE%212911574338%21X%211%210%21n_tag%3A-29919%3Bd%3A87806434%3Bm03_new_user%3A-29895&curPageLogUid=T5dw2yNWnfpz&utparam-url=scene%3Asearch%7Cquery_from%3A%7Cx_object_id%3A1005006579026411%7C_p_origin_prod%3A

The glass dome was bought on AliExpress

The 12 V mini step up module was bought on AliExpress: https://de.aliexpress.com/item/1005005835656823.html?spm=a2g0o.order_detail.order_detail_item.9.743a6368CpXI0D&gatewayAdapt=glo2deu

The ESC module was bought an AliExpress: https://de.aliexpress.com/item/1005008034100991.html?spm=a2g0o.order_list.order_list_main.67.53175c5fBHUX8V&gatewayAdapt=glo2deu (8 A Version)

The BLDC motor was bought on AliExpress: https://de.aliexpress.com/item/1005006110056005.html?spm=a2g0o.order_list.order_list_main.57.53175c5fBHUX8V&gatewayAdapt=glo2deu

The 16x16 LED Matrix was assembled by JLCPCB PCBA service
The other circuit boards where assembled by hand.

The wireless coils where glued into the holders (make sure to remove the small piece of tape beforehand) with superglue. 
