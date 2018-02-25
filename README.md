## About
**Wuabit Dust** is a password/key usb generator **targeting to be the most affordable hardware based generator device in the world**, period.

It's using one of the most **tiny/cheapest** arduino boards: The open-source [Digispark board](https://digistump.com/wiki/digispark). You can find it for **less than 2€** on internet.

Here is a photo of the device:

![Original Digispark](https://s3.amazonaws.com/digistump-resources/images/l/61e2f14edffc1edfa2685963155b0d33.jpg)


## Features

* Plug-and-play: As it works as a standard USB keyboard
* Stores key/pass in internal [eeprom](https://en.wikipedia.org/wiki/EEPROM) memory with more than 100,000 cycles of writes assured.
* Uses the ATtiny85 microcontroller from Atmel, one of the most ubiquitous 8-bit mcu.

## How it works

**Wuabit Dust** generates keys/passwords by simply connecting it to a free USB port (works even in Android with USB-OTG).

# Current Versions:

* [vSHA-256](wuabit-dust-sha256): Generates **one single key**.

More versions to come, stay tuned to this repo!

## Security warning

This device generates a key in hardware, but writes it via usb-keyboard emulation, so if the target machine is compromised with a keylogger then the key can be stolen. The same if it's sent to a compromised/evil online service etc...


## License and copyright

This project is licensed under the terms given in the **LICENSE** file in each project.

(c) 2018 by [Manuel Polo](https://about.me/mrmx) a.k.a @[mrmx](https://twitter.com/mrmx)


