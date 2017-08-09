# nanobaro

Nanobaro is a simple barometric pressure and temperature sensor using Arduino Nano and the BMP180 digital pressure sensor. It outputs data via the Nano's built in USB-serial interface in NMEA0183 syntax. Works directly with OpenCPN's dashboard plugin (barograph history) or any other NMEA data consumers.

## NMEA output

Nanobaro outputs one NMEA0183 `XDR` (transducer measurement) sentence with talker id `WI` (weather instruments) every 10 seconds. Here's an example:

    $WIXDR,C,24.59,C,TEMP,P,1.02412,B,BARO*5E

This reports a temperature reading of 24.59 degree Celsius from a transducer with the id `TEMP` and a pressure reading of 1.02412 Bar (1024.12 millibar) from a transducer with the id `BARO`. The SI Pascal unit should really be used here, but was found to be unsupported by the targeted OpenCPN, so Bar was used instead.

Nanobaro will also output human readable `TXT` sentences at startup and if anything goes wrong.

## Hardware

The hardware is very simple. It's a BMP180 chip connected with four wires to a I²C clock/data pair as well as ground and 5V on the Arduino. More information and photos can be found on [my related blog post](https://sdfjkl.org/blog/2017-08-09-nanobaro/).

## Arduino Nano Firmware

Program `nanobar.ino` onto your Nano. You will need the **Wire** (I²C) library included with the Arduino package as well as the [SFE_BMP180 library from Sparkfun](https://github.com/sparkfun/BMP180_Breakout).

## Software

There is no other software included. I'm using OpenCPN's dashboard plugin, with a separate dashboard containing only the **Barometric history** widget. You can either directly connect to the Nano's USB-serial device with OpenCPN, or via a NMEA multiplexer such as [kplex](http://www.stripydog.com/kplex/), which lets you easily make the data available to multiple devices on a wireless or wired network.