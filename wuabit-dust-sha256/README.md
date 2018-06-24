## About
**Wuabit Dust** *vSHA-256* is a 256bits password/key usb generator **targeting to be the most affordable hardware based generator device in the world**, period.

It's using one of the most **tiny/cheapest** arduino boards: The open-source [Digispark board](https://digistump.com/wiki/digispark). You can find it for **less than 2€** on internet.

Here is a photo of the device:

![Original Digispark](https://s3.amazonaws.com/digistump-resources/images/l/61e2f14edffc1edfa2685963155b0d33.jpg)

Pull requests are always welcome.

## Features

* Plug-and-play: As it works as a standard USB keyboard
* 32 bytes of key length generation (256 bits)
* High entropy of > 7.99 bits per byte (see [tests](tests/) and compare to eg. [Trezor random tests](https://github.com/trezor/rng-test)).
* Stores key/pass in internal [eeprom](https://en.wikipedia.org/wiki/EEPROM) memory with more than 100,000 cycles of writes assured.
* Uses the [ATtiny85](https://www.microchip.com/wwwproducts/en/ATtiny85) microcontroller from [Atmel](https://en.wikipedia.org/wiki/Atmel) now [MicroChip](https://www.microchip.com), one of the most ubiquitous 8-bit mcu.

## How it works

This version of **Wuabit Dust** generates **ONE UNIQUE KEY** and stores it in the permanent memory (eeprom) of the ATtiny85. But it's able to generate **MANY KEYS**.  **To force the device to generate another key, just leave it connected for a while (1min)**.

* Plug it to an USB port (works even in Android with USB-OTG)
* First time will check if its initialized.
* Will collect enough entropy (randomness) using an internal hardware clock (the WDT): 256bits total.
* The result will pass a SHA-256 hash function to [whitening](https://en.wikipedia.org/wiki/Whitening_transformation) the initial entropy.
* The result hash will be stored to eeprom memory as the result key. Also store a flag to know it's initialized/ready.
* The generated key will be sent as keyboard keystrokes of characters from 0-9 and a-f (hex codes).

The next time the device is plugged-in the same stored key will be sent.

If device is left connected after it sent the key from eeprom, a timeout starts and the onboard led will begin to flash slowly, then faster and faster until regenerates another key, **overwriting the previous stored key**.

## Security warning

This device generates a key in hardware, but writes it via usb-keyboard emulation, so if the target machine is compromised with a keylogger then the key can be stolen. The same if it's sent to a compromised/evil online service etc...

## Installation/Build

The installation and build process is managed multiplatform by [PlatformIO](https://platformio.org) [CORE](http://docs.platformio.org/en/latest/core.html) or [IDE](http://docs.platformio.org/en/latest/ide/pioide.html). It can be easily [installed](http://docs.platformio.org/page/installation.html) in any OS with python.

Once installed simply execute (on cli core):

> platformio run -t upload

Or click _upload_ via the IDE. 


## Credits 

[Entropy](https://sites.google.com/site/astudyofentropy/home) A great WDT based entropy generator for the [Atmel AVR](https://en.wikipedia.org/wiki/Atmel_AVR) series 8-bit microcontrollers found in Arduino boards.

[usha256](https://github.com/hypoactiv/usha256) a tiny [SHA-256](https://en.wikipedia.org/wiki/SHA-2) message digest implementation for AVRs.

## License and copyright

This project is licensed under the GNU GPL v3 (see [license](LICENSE)).

(c) 2018 by [Manuel Polo](https://about.me/mrmx) a.k.a @[mrmx](https://twitter.com/mrmx)